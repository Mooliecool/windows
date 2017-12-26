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
public class Co1115Parse
{
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co1115Parse  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String[] str5ArrExcTestcase =  
   {
     " 7l "  
     ,"0x100000002"  
     ,"5-"
   };
   String[] str3ArrGoodTestcase = 
   {
     "22"
     ,"041"  
     ,"089"  
     ,"-0"
     ,"0"
     ,"07"
     ,"     7   "
   };
   int[] in4ArrExpected = 
   {
     22
     ,41  
     ,89
     ,0
     ,0
     ,7
     ,7
   };
   int[] in4ArrResult = null; 
   int in4w1 = -2;
   int aa = 0;
   try
     {
     do
       {
       if ( in4ArrExpected.Length != str3ArrGoodTestcase.Length )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_045eg test program is flawed."  );
	 }
       in4ArrResult = new int[ in4ArrExpected.Length ];
       for ( int xx = 0 ;xx < in4ArrResult.Length ;xx++ )
	 {
	 in4ArrResult[xx] = -( 1000 + xx );
	 }
       strLoc="Loc_100mm";
       for ( aa = 0 ;aa < str3ArrGoodTestcase.Length ;aa++ )
	 {
	 try
	   {
	   ++iCountTestcases;
	   in4ArrResult[aa] = Int32.Parse( str3ArrGoodTestcase[aa] );
	   if ( in4ArrResult[aa] != in4ArrExpected[aa] )
	     {
	     ++iCountErrors;
	     Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_839vd ,aa=="+ aa +" ,in4ArrResult[aa]=="+ in4ArrResult[aa]  );
	     }
	   }
	 catch ( FormatException fexc )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_837sk ,aa=="+ aa +" ,fexc=="+ fexc  );
	   }
	 catch ( Exception excep1 )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_836wz ,aa=="+ aa +" ,excep1=="+ excep1  );
	   }
	 } 
       strLoc="Loc_200hh";
       for ( aa = 0 ;aa < str5ArrExcTestcase.Length ;aa++ )
	 {
	 try
	   {
	   ++iCountTestcases;
	   in4w1 = Int32.Parse( str5ArrExcTestcase[aa] );
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_461af ,aa=="+ aa +" ,in4w1=="+ in4w1  );
	   }
	 catch ( FormatException  )
	   {
	   continue;
	   }
	 catch ( Exception excep1 )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_463ho ,aa=="+ aa +" ,excep1=="+ excep1  );
	   }
	 } 
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1115Parse) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer4\\Co1115Parse.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer4\\Co1115Parse.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   Co1115Parse cbA = new Co1115Parse();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1115Parse) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1115Parse.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
