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
public class Co1243ToBoolean
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1243ToBoolean  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int inStartOffset = -2;
   bool bo2 = false;
   bool bo4ExpectedResult = true;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100ku";
       bo2 = true;
       bo4ExpectedResult = false;  
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x11;
       byArr3[ 1] = (byte)0x00;
       byArr3[ 2] = (byte)0x00;
       byArr3[ 3] = (byte)0x00;
       byArr3[ 4] = (byte)0x00;
       byArr3[ 5] = (byte)0x22;
       bo2 = BitConverter.ToBoolean( byArr3 ,1 );  
       ++iCountTestcases;
       if ( bo2 != bo4ExpectedResult )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_101as!"  );
	 }
       strLoc="Loc_110bv";
       bo2 = false;
       bo4ExpectedResult = true;
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x11;
       byArr3[ 1] = (byte)0x03;
       byArr3[ 2] = (byte)0x03;
       byArr3[ 3] = (byte)0x03;
       byArr3[ 4] = (byte)0x80;
       byArr3[ 5] = (byte)0x22;
       bo2 = BitConverter.ToBoolean( byArr3 ,1 );  
       ++iCountTestcases;
       if ( bo2 != bo4ExpectedResult )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_111as!"  );
	 }
       strLoc="Loc_200xt";
       bo2 = true;
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x11;
       byArr3[ 1] = (byte)0x03;
       byArr3[ 2] = (byte)0x03;
       byArr3[ 3] = (byte)0x03;
       byArr3[ 4] = (byte)0x80;
       byArr3[ 5] = (byte)0x22;
       try
	 {
	 ++iCountTestcases;
	 bo2 = BitConverter.ToBoolean( byArr3 ,byArr3.Length  );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_201wl!  bo2==" + bo2  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_202jc!  exc.ToString()==" + exc.ToString()  );
	 }
       strLoc="Loc_210ms";
       bo2 = true;
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x11;
       byArr3[ 1] = (byte)0x03;
       byArr3[ 2] = (byte)0x03;
       byArr3[ 3] = (byte)0x03;
       byArr3[ 4] = (byte)0x80;
       byArr3[ 5] = (byte)0x22;
       Random random = new Random();
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   bo2 = BitConverter.ToBoolean( byArr3 ,random.Next(Int32.MinValue, 0) );  
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_211wl!  bo2==" + bo2  );
	   }
	 catch ( ArgumentOutOfRangeException argexc )
	   {}
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_212jc!  exc.ToString()==" + exc.ToString()  );
	   }
	 }
       ++iCountTestcases;
       try
	 {
	 bo2 = BitConverter.ToBoolean( byArr3 ,Int32.MaxValue );
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
	 bo2 = BitConverter.ToBoolean( byArr3 ,Int32.MinValue );
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
       strLoc="Loc_230jl";
       bo2 = true;
       byArr3 = null;
       try
	 {
	 ++iCountTestcases;
	 bo2 = BitConverter.ToBoolean( byArr3 ,0 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_231wl!  bo2==" + bo2  );
	 }
       catch ( ArgumentNullException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_232jc!  exc.ToString()==" + exc.ToString()  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1243ToBoolean) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1243ToBoolean.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1243ToBoolean.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1243ToBoolean cbA = new Co1243ToBoolean();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1243ToBoolean) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1243ToBoolean.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
