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
public class Co4059Min_int
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4059Min_int.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int intVal1 = 0;
   int intVal2 = 0;
   int intexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       intVal1 = (int)2147483646;  
       intVal2 = (int)2147483647;  
       intexpectedValue = (int)2147483646;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( " intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)2147483647;  
       intVal2 = (int)2147483646;  
       intexpectedValue = (int)2147483646;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)-2147483647;  
       intVal2 = (int)-2147483648;  
       intexpectedValue = (int)-2147483648;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)-2147483648;  
       intVal2 = (int)-2147483647;  
       intexpectedValue = (int)-2147483648;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ve73, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)2147483647;   
       intVal2 = (int)-2147483648;  
       intexpectedValue = (int)-2147483648;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)-2147483648;  
       intVal2 = (int)2147483647;  
       intexpectedValue = (int)-2147483648;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       intVal1 = (int)0;  
       intVal2 = (int)0;  
       intexpectedValue = (int)0;
       if ( Math.Min( intVal1, intVal2) != intexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Min value did not come up, Expected intVal1 == " + intVal1);
	 sblMsg.Append( "intVal2 == " + intVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       }
     catch ( Exception Exc )
       {
       ++iCountErrors;
       sblMsg.Length =  99 ;
       sblMsg.Append( "POINTTOBREAK: find E_f3h5, Generic Exception Caught, Exc.ToString() == " );
       sblMsg.Append( Exc.ToString() );
       Console.Error.WriteLine( sblMsg.ToString() );
       break;
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Math\\Co4059Min_int.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4059Min_int.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4059Min_int.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4059Min_int cbA = new Co4059Min_int();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4059Min_int.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
