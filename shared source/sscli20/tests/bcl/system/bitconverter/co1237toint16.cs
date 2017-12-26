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
public class Co1237ToInt16
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1237ToInt16  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   short sh2y = -2;
   short sh2z = 0;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100ku";
       sh2y = (short)0x1234;
       sh2z = (short)-2;
       byArr3 = new byte[9];
       if (BitConverter.IsLittleEndian) {
           byArr3[3] = (byte)0x34;
           byArr3[4] = (byte)0x12;
       }
       else {
           byArr3[4] = (byte)0x34;
           byArr3[3] = (byte)0x12;
       }
       sh2z = BitConverter.ToInt16( byArr3 ,3 );
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_101as!  sh2z==" + sh2z  );
	 }
       strLoc="Loc_120ku";
       sh2y = unchecked((short)0x8765);
       sh2z = (short)-2;
       byArr3 = new byte[9];
       if (BitConverter.IsLittleEndian) {
           byArr3[3] = (byte)0x65;
           byArr3[4] = (byte)0x87;
       }
       else {
           byArr3[4]= (byte)0x65;
           byArr3[3]= (byte)0x87;
       }
       sh2z = BitConverter.ToInt16( byArr3 ,3 );
       ++iCountTestcases;
       if ( sh2z != sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_121as!  sh2z==" + sh2z  );
	 }
       strLoc="Loc_110ku";
       sh2y = (short)0x123;
       sh2z = (short)-2;
       byArr3 = new byte[1];
       byArr3[0] = (byte)0x12;
       try
	 {
	 ++iCountTestcases;
	 sh2z = BitConverter.ToInt16( byArr3 ,0 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_113yj!  sh2z==" + sh2z  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_112qd!  sh2z==" + sh2z  );
	 }
       ++iCountTestcases;
       if ( sh2z == sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_111as!  sh2z==" + sh2z  );
	 }
       strLoc="Loc_130ku";
       sh2y = (short)0x1234;
       sh2z = (short)-2;
       byArr3 = new byte[5];
       byArr3[3] = (byte)0x34;
       byArr3[4] = (byte)0x12;
       try
	 {
	 ++iCountTestcases;
	 sh2z = BitConverter.ToInt16( byArr3 ,4 );
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_133yj!  sh2z==" + sh2z  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_132qd!  sh2z==" + sh2z  );
	 }
       ++iCountTestcases;
       if ( sh2z == sh2y )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_131as!  sh2z==" + sh2z  );
	 }
       ++iCountTestcases;
       try
	 {
	 sh2z = BitConverter.ToInt16( null ,1 );
	 ++iCountErrors;
	 Console.WriteLine("Err_3947tsfg! Exception not thrown");
	 }
       catch(ArgumentNullException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine("Err_0745rtgf! Wrong exception thrown, " + ex.GetType().Name);
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
	   sh2z = BitConverter.ToInt16( byArr3 ,randValue);
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
	 sh2z = BitConverter.ToInt16( byArr3 ,Int32.MaxValue );
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
	 sh2z = BitConverter.ToInt16( byArr3 ,Int32.MinValue );
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
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1237ToInt16) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1237ToInt16.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1237ToInt16.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1237ToInt16 cbA = new Co1237ToInt16();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1237ToInt16) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1237ToInt16.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
