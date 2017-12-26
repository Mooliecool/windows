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
public class Co4258ToString
{
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co4258ToString.cs  runTest() started." );
   StringBuilder sblMsg = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   do
     {
     String stringOne = null;
     Single singleOne = ( (float)3 );
     stringOne = singleOne.ToString();
     ++iCountTestcases;
     if ( ! stringOne.Equals( singleOne.ToString() ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_13dd (Co4258ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_13dd ,Co4258ToString).  stringOne==" );
       sblMsg.Append( stringOne );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     stringOne = null;
     singleOne = ( (float)-3 );
     stringOne = singleOne.ToString();
     ++iCountTestcases;
     if ( ! stringOne.Equals( singleOne.ToString() ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_lo36 (Co4258ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_lo36 ,Co4258ToString).  stringOne==" );
       sblMsg.Append( stringOne );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     stringOne = null;
     singleOne = ( (float)3.4028235e+38 );
     stringOne = singleOne.ToString();
     ++iCountTestcases;
     if ( ! stringOne.Equals( singleOne.ToString() ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_26jr (Co4258ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_26jr ,Co4258ToString).  stringOne==" );
       sblMsg.Append( stringOne );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     stringOne = null;
     singleOne = ( (float)-3.4028235e+38);
     stringOne = singleOne.ToString();
     ++iCountTestcases;
     if ( ! stringOne.Equals( singleOne.ToString() ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_49ht (Co4258ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_49ht ,Co4258ToString).  stringOne==" );
       sblMsg.Append( stringOne );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     stringOne = null;
     singleOne = ( (float)0 );
     stringOne = singleOne.ToString();
     ++iCountTestcases;
     if ( ! stringOne.Equals( singleOne.ToString() ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_49ht (Co4258ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_49ht ,Co4258ToString).  stringOne==" );
       sblMsg.Append( stringOne );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Single\\Co4258ToString.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4258ToString.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4258ToString.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4258ToString cbA = new Co4258ToString();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4258ToString.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
