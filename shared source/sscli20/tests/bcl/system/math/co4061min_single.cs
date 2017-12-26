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
public class Co4061Min_single
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4061Min_single.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   float floatVal1 = 0;
   float floatVal2 = 0;
   float floatexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       floatVal1 = (float)9223372036854775806f;  
       floatVal2 = (float)9223372036854775807f;  
       floatexpectedValue = (float)9223372036854775806f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( " floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)9223372036854775807f;  
       floatVal2 = (float)9223372036854775806f;  
       floatexpectedValue = (float)9223372036854775807f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( " floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-9223372036854775807f;  
       floatVal2 = (float)-9223372036854775808f;  
       floatexpectedValue = (float)-9223372036854775808f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-9223372036854775808f;  
       floatVal2 = (float)-9223372036854775807f;  
       floatexpectedValue = (float)-9223372036854775808f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ve73, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)9223372036854775807f;   
       floatVal2 = (float)-9223372036854775808f;  
       floatexpectedValue = (float)-9223372036854775808f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-9223372036854775808f;  
       floatVal2 = (float)9223372036854775807f;   
       floatexpectedValue = (float)-9223372036854775808f;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)0;  
       floatVal2 = (float)0;  
       floatexpectedValue = (float)0;
       if ( Math.Min( floatVal1, floatVal2) != floatexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Min value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
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
     Console.Error.Write( "Math\\Co4061Min_single.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4061Min_single.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4061Min_single.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4061Min_single cbA = new Co4061Min_single();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4061Min_single.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
