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
public class Co1117CompareTo
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1117CompareTo  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int in4b = -12;
   Int32 int4a = 55;
   Object o1;
   int retValue;
   try
     {
       strLoc="Loc_120au";
       in4b = -12;
       o1 = 66;
       in4b = int4a.CompareTo( o1);
       ++iCountTestcases;
       if ( !( in4b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_337uf!  in4b=="+ in4b  );
	 }
       in4b = -12;
       o1=55;
       in4b = int4a.CompareTo( o1 );
       ++iCountTestcases;
       if ( !( in4b == -0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_248ak!  in4b=="+ in4b  );
	 }
       in4b = -12;
       o1 = -77;
       in4b = int4a.CompareTo( o1 );
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_411wq!  in4b=="+ in4b  );
	 }
       strLoc="Loc_201ax";
       in4b = -12;
       in4b = int4a.CompareTo( null );  
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_713tw!  in4b=="+ in4b  );
	 }
       strLoc="Loc_202bx";
       in4b = -12;
       in4b = int4a.CompareTo( (Type)null );  
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_714rw!  in4b=="+ in4b  );
	 }
       strLoc="Loc_205ex";
       in4b = -12;
       in4b = ( -0 ).CompareTo( null );
       ++iCountTestcases;
       if ( !( in4b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_716mw!  in4b=="+ in4b  );
	 }
       strLoc="Loc_301zv";
       in4b = -12;
       o1 = Int32.MinValue + 1;
       in4b = ( Int32.MinValue ).CompareTo( o1 );
       ++iCountTestcases;
       if ( !( in4b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_958je!  in4b=="+ in4b  );
	 }
       strLoc="Loc_303uv";
       Console.Error.WriteLine( "Info Inf_774dx.  (Integer4.MinValue-Integer4.MaxValue)=="+ unchecked(Int32.MinValue-Int32.MaxValue) );
       strLoc="Loc_302yv";
       in4b = -12;
       o1 = Int32.MaxValue;
       in4b = ( Int32.MinValue ).CompareTo(o1 );
       ++iCountTestcases;
       if ( !( in4b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_842sc!  in4b=="+ in4b  );
	 }
     Object[] os = {Int16.MaxValue, Int64.MaxValue, UInt16.MaxValue, UInt32.MaxValue, UInt64.MaxValue, "Hello World"};
     foreach(Object oo in os)
     {
         in4b = 0;
         iCountTestcases++;
         try{
             retValue = in4b.CompareTo(oo);
             iCountErrors++;
             Console.WriteLine("Err_3487sdg! No excewpiton thrown : {0}", retValue);
         }catch(ArgumentException){
         }catch(Exception ex){
         iCountErrors++;
         Console.WriteLine("Err_3487sdg! Wrong excewpiton thrown : {0}", ex);
         }
     }
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1117CompareTo) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer4\\Co1117CompareTo.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer4\\Co1117CompareTo.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co1117CompareTo cbA = new Co1117CompareTo();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1117CompareTo) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1117CompareTo.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
