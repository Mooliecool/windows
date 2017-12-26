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
public class Co1369Finally_Block
{
 private static int _i = 0;
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0;
   System.Console.Error.WriteLine( "Exception- Co1369F_B main started." );
   System.Console.Error.WriteLine( "Caution,  describes cpu 100% finally block problem." );
   SetSeed( 111 );
   iCountTestcases++;
   AAA();
   System.Console.Error.WriteLine( GetSeed() );
   if (GetSeed() != 111)
     {
     iCountErrors++;
     System.Console.Error.WriteLine( "Error GetSeed() did not return 111, Returned" + GetSeed() );
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.    ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!    ,iCountErrors=="+iCountErrors);
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;
   Co1369Finally_Block cbA = new Co1369Finally_Block ();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine("Co1369Finally_Block : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine( "FAiL!  Co1369Finally_Block");
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
 public static bool AAA()
   {
   int i = GetSeed();
   try
     {
     SetSeed( 3 );
     if ( i == 111 )
       return true;
     }
   catch ( System.Exception)
     {
     return false;
     }
   finally
     {
     SetSeed( i ); 
     }
   return true;
   }
 public static int GetSeed()
   {
   return _i;
   }
 public static void SetSeed( int i )
   {
   _i = i;
   }
}
