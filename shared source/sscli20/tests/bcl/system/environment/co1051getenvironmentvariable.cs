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
public class Co1051GetEnvironmentVariable
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1051GetEnvironmentVariable  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str2VarValue = null;
   String str3VarName = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_200aa";
#if PLATFORM_UNIX
       str3VarName = "HOME";
#else
       str3VarName = "TEMP";
#endif
       str2VarValue = null;
       str2VarValue = Environment.GetEnvironmentVariable( str3VarName );
       ++iCountTestcases;
       if ( str2VarValue == null )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_402ky!  str3VarName=="+ str3VarName  );
	 }
       strLoc="Loc_240bb";
       str3VarName = "no_such_envar_name";
       str2VarValue = "nonNull foil";
       str2VarValue = Environment.GetEnvironmentVariable( str3VarName );
       ++iCountTestcases;
       if ( str2VarValue != null )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_403my!  str3VarName=="+ str3VarName +" ,str2VarValue=="+ str2VarValue  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_888yy! (Co1051GetEnvironmentVariable) strLoc=="+ strLoc +" ,exc_general==" + exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1051GetEnvironmentVariable.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1051GetEnvironmentVariable.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1051GetEnvironmentVariable cbA = new Co1051GetEnvironmentVariable();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1051GetEnvironmentVariable) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1051GetEnvironmentVariable.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
