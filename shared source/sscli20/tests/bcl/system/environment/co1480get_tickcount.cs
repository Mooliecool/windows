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
using System.Threading;
public class Co1480get_TickCount
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "System- Co1480get_TickCount runTest started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int iErrorBits = 0;
   int iCountA = 0;
   int iCountB = 0;
   iCountA = Environment.TickCount;
   try { Thread.Sleep( 1050 ); } catch ( Exception exc ) {}
   iCountB = Environment.TickCount;
   Console.Error.Write( "Co1480get_TickCount Info I_821wi.  B-A==" );
   Console.Error.WriteLine( iCountB - iCountA ); 
   ++iCountTestcases;
   if (
       ( iCountB - iCountA ) < 1001
       ||  ( iCountB - iCountA ) > 1100
       )
     {
     iCountErrors += 1;
     Console.Error.WriteLine( "Co1480get_TickCount Error E_449gz!" );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "System- Co1480get_TickCount: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co1480get_TickCount iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "System- Co1480get_TickCount: FAiL!" );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co1480get_TickCount o2Co1480get_TickCount = new Co1480get_TickCount();
     bResult = o2Co1480get_TickCount.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "System- Co1480get_TickCount main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
