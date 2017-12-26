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
public class Co1118CompareTo
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1118CompareTo  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   long lo8b = -12;
   Int64 int8a = 55;
   Object o1;
   int retValue;
   try
     {
       strLoc="Loc_120au";
       strLoc="Loc_121eu";
       lo8b = -12;
       o1 = (Int64)66;
       lo8b = int8a.CompareTo( o1 );
       ++iCountTestcases;
       if ( !( lo8b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_337uf!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_122fu";
       lo8b = -12;
       o1 = 55L;
       lo8b = int8a.CompareTo( o1 );
       ++iCountTestcases;
       if ( !( lo8b == -0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_248ak!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_123gu";
       lo8b = -12;
       o1 = (Int64)(-77);
       lo8b = int8a.CompareTo( o1 );
       ++iCountTestcases;
       if ( !( lo8b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_411wq!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_201ax";
       lo8b = -12;
       lo8b = int8a.CompareTo( null );  
       ++iCountTestcases;
       if ( !( lo8b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_713tw!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_202bx";
       lo8b = -12;
       lo8b = int8a.CompareTo( (Type)null );  
       ++iCountTestcases;
       if ( !( lo8b > 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_714rw!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_301zv";
       lo8b = -12;
       o1 = ( Int64.MinValue + 1 );
       lo8b = ( Int64.MinValue ).CompareTo( o1 );
       ++iCountTestcases;
       if ( !( lo8b < 0 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_958je!  lo8b=="+ lo8b  );
	 }
       strLoc="Loc_303uv";
       Console.Error.WriteLine( "Info Inf_774dx.  (Integer8.MinValue-Integer8.MaxValue)=="+ unchecked(Int64.MinValue-Int64.MaxValue) );
       strLoc="Loc_302yv";
       lo8b = -12;
       o1 = Int64.MaxValue;
       lo8b = ( Int64.MinValue ).CompareTo( o1 );
       ++iCountTestcases;
       if ( !( lo8b < 0 ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_842sc!  lo8b=="+ lo8b  );
       }
       strLoc="Loc_400hh";
        Object[] os = {Int16.MaxValue, Int32.MaxValue, UInt16.MaxValue, UInt32.MaxValue, UInt64.MaxValue, "Hello World"};
        foreach(Object oo in os)
        {
            int8a = 0L;
            iCountTestcases++;
            try{
                retValue = int8a.CompareTo(oo);
                
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
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1118CompareTo) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer8\\Co1118CompareTo.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer8\\Co1118CompareTo.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co1118CompareTo cbA = new Co1118CompareTo();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1118CompareTo) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1118CompareTo.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
