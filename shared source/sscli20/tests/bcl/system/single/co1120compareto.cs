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
public class Co1120CompareTo
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1120CompareTo  runTest() started." );
   Console.WriteLine( "Locale insensitive" );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int in4b = -12;
   Single sing3 = (float)55.3;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_120au";
       in4b = -12;
       in4b = sing3.CompareTo( (float)66.3 );
       ++iCountTestcases;
       if ( !( in4b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_337uf!  in4b=="+ in4b  );
	 }
       in4b = -12;
       in4b = sing3.CompareTo( (float)55.3 );
       ++iCountTestcases;
       if ( !( in4b == -0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_248ak!  in4b=="+ in4b  );
	 }
       in4b = -12;
       in4b = sing3.CompareTo( (float)-77.3 );
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_411wq!  in4b=="+ in4b  );
	 }
       strLoc="Loc_201ax";
       in4b = -12;
       in4b = sing3.CompareTo( null );  
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_713tw!  in4b=="+ in4b  );
	 }
       strLoc="Loc_202bx";
       in4b = -12;
       in4b = sing3.CompareTo( (Type)null );  
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_714rw!  in4b=="+ in4b  );
	 }
       strLoc="Loc_204dx";
       strLoc="Loc_301zv";
       in4b = -12;
       in4b = ( Single.MinValue ).CompareTo( ( Single.MinValue + 1 ) );  
       ++iCountTestcases;
       if ( !( in4b == 0 ) )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_958je!  in4b=="+ in4b  );
	 }
       strLoc="Loc_308xj";
       in4b = -12;
       in4b = ( Single.MinValue ).CompareTo( ( Single.MinValue - ( Single.MinValue / (float)100.0 ) ) );
       ++iCountTestcases;
       if ( !( in4b < 0 ) )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_680db!  in4b=="+ in4b  );
	 }
       strLoc="Loc_303uv";
       Console.Error.WriteLine( "Info Inf_774dx.  (Single.MinValue-Single.MaxValue)=="+ (Single.MinValue-Single.MaxValue) );
       strLoc="Loc_302yv";
       in4b = -12;
       in4b = ( Single.MinValue ).CompareTo( Single.MaxValue );  
       ++iCountTestcases;
       if ( !( in4b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_842sc!  in4b=="+ in4b  );
	 }
       strLoc="Loc_301yv";
       in4b = -12;
       in4b = ( Single.NaN ).CompareTo( null );
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_777sc!  in4b=="+ in4b  );
	 }
       strLoc = "Loc_28hcd";
       in4b = (Single.NaN).CompareTo(Single.NaN);
       ++iCountTestcases;
       if(in4b != 0)
	 {
	 ++iCountErrors;
	 Console.WriteLine("POINTTOBREAK: Error_9388c! in4b=="+in4b);
	 }
       strLoc = "Loc_0039c";
       in4b = ((Single)4.4).CompareTo(Single.NaN);
       ++iCountTestcases;
       if(!(in4b > 0) )
	 {
	 ++iCountErrors;
	 Console.WriteLine("POINTTOBREAK: Error_18yf8! in4b=="+in4b);
	 }
       strLoc="Loc_400hh";
       try
	 {
	 in4b = -12;
	 in4b = sing3.CompareTo( (double)22.3 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_522yh!  in4b=="+ in4b  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_636pi!  excep1=="+ excep1  );
	 }
       strLoc="Loc_440hh";
       try
	 {
	 in4b = sing3.CompareTo( new Object());  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_542yh!  in4b=="+ in4b  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_646pi!  excep1=="+ excep1  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1120CompareTo) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Single\\Co1120CompareTo.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Single\\Co1120CompareTo.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co1120CompareTo cbA = new Co1120CompareTo();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1120CompareTo) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1120CompareTo.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
