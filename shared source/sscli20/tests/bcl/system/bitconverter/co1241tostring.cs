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
public class Co1241ToString
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1241ToString  runTest started." );
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
       byArr3 = new byte[16];
       byArr3[ 1] = (byte)0x49;  
       byArr3[ 2] = (byte)0x4A;
       str3 = BitConverter.ToString( byArr3 ,1 ,2 );  
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
	 str3 = BitConverter.ToString( byArr3 ,1 ,3 );  
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
       strLoc="Loc_210bz";
       str3 = "wrong 586201";
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x41;  
       byArr3[ 1] = (byte)0x42;
       byArr3[ 2] = (byte)0x43;
       byArr3[ 3] = (byte)0x44;
       Random random = new Random();
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   str3 = BitConverter.ToString( byArr3 ,random.Next(Int32.MinValue, 0) ,3 );  
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_211wl!  str3==" + str3  );
	   }
	 catch ( ArgumentOutOfRangeException argexc )
	   {}
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_212jc!  exc.ToString()==" + exc.ToString()  );
	   }
	 }
       strLoc="Loc_220bz";
       str3 = "wrong 586201";
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x41;  
       byArr3[ 1] = (byte)0x42;
       byArr3[ 2] = (byte)0x43;
       byArr3[ 3] = (byte)0x44;
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   str3 = BitConverter.ToString( byArr3 ,0 ,random.Next(Int32.MinValue, 0) );  
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_221wl!  str3==" + str3  );
	   }
	 catch ( ArgumentOutOfRangeException argexc )
	   {}
	 catch ( Exception exc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_222jc!  exc.ToString()==" + exc.ToString()  );
	   }
	 }
       strLoc="Loc_230bz";
       str3 = "wrong 586201";
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x41;  
       byArr3[ 1] = (byte)0x42;
       byArr3[ 2] = (byte)0x43;
       byArr3[ 3] = (byte)0x44;
       byArr3[ 4] = (byte)0x45;
       try
	 {
	 ++iCountTestcases;
	 str3 = BitConverter.ToString( byArr3 ,0 ,byArr3.Length + 2 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_231wl!  str3==" + str3  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_232jc!  exc.ToString()==" + exc.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 str3 = BitConverter.ToString( byArr3 ,0 ,Int32.MaxValue);  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_231wl!  str3==" + str3  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_232jc!  exc.ToString()==" + exc.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 str3 = BitConverter.ToString( byArr3 ,Int32.MaxValue ,2);  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_231wl!  str3==" + str3  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_232jc!  exc.ToString()==" + exc.ToString()  );
	 }
       strLoc="Loc_240bz";
       str3 = "wrong 586201";
       byArr3 = null;
       try
	 {
	 ++iCountTestcases;
	 str3 = BitConverter.ToString( byArr3 ,0 ,byArr3.Length + 2 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_241wl!  str3==" + str3  );
	 }
       catch ( NullReferenceException nrexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_242jc!  exc.ToString()==" + exc.ToString()  );
	 }
       strLoc="Loc_300mn";
       str2 = "";
       str3 = "wrong 24350";
       byArr3 = new byte[16];
       byArr3[ 1] = (byte)0x44;
       byArr3[ 2] = (byte)0x45;
       byArr3[ 3] = (byte)0x46;
       str3 = BitConverter.ToString( byArr3 ,0 ,0 );  
       ++iCountTestcases;
       if ( str3.Equals( str2 ) == false )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_301as!  str3==" + str3  );
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
       str3 = BitConverter.ToString( byArr3 ,inStartOffset ,byArr3.Length );  
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
       strLoc="Loc_410ys";
       inStartOffset = 2;  
       str2 = "43-44-45-46-00-00-00-00-00-00-00-00-00-00";
       str3 = "wrong 24350";
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x41;  
       byArr3[ 1] = (byte)0x42;
       byArr3[ 2] = (byte)0x43;
       byArr3[ 3] = (byte)0x44;
       byArr3[ 4] = (byte)0x45;
       byArr3[ 5] = (byte)0x46;
       str3 = BitConverter.ToString( byArr3 ,inStartOffset ,byArr3.Length-inStartOffset );  
       ++iCountTestcases;
       if ( str3.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_411as!  str3==" + str3  );
	 }
       ++iCountTestcases;
       if ( str3.Length == byArr3.Length - inStartOffset )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_412yo!  str3==" + str3  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1241ToString) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1241ToString.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1241ToString.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1241ToString cbA = new Co1241ToString();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1241ToString) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1241ToString.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
