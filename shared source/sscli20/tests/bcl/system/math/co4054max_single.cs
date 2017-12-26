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
public class Co4054Max_single
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4054Max_single.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   float floatVal1 = 0;
   float floatVal2 = 0;
   float flexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       floatVal1 = (float)2147483646;  
       floatVal2 = (float)2147483647;  
       flexpectedValue = (float)2147483647;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)2147483647;  
       floatVal2 = (float)2147483646;  
       flexpectedValue = (float)2147483647;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-2147483646;  
       floatVal2 = (float)-2147483647;  
       flexpectedValue = (float)-2147483646;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-2147483647;  
       floatVal2 = (float)-2147483646;  
       flexpectedValue = (float)-2147483646;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)2147483647;  
       floatVal2 = (float)-2147483647;  
       flexpectedValue = (float)2147483647;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)-2147483647;  
       floatVal2 = (float)2147483647;  
       flexpectedValue = (float)2147483647;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       floatVal1 = (float)0;  
       floatVal2 = (float)0;  
       flexpectedValue = (float)0;
       if ( Math.Max( floatVal1, floatVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Max value did not come up, Expected floatVal1 == " + floatVal1);
	 sblMsg.Append( "floatVal2 == " + floatVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       if(!Single.NaN.Equals(Math.Max(Single.NaN, 5f))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
       }
       if(!Single.NaN.Equals(Math.Max(5f, Single.NaN))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
       }
       if(!Single.NaN.Equals(Math.Max(Single.NaN, Single.NaN))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
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
     Console.Error.Write( "Math\\Co4054Max_single.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4054Max_single.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4054Max_single.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4054Max_single cbA = new Co4054Max_single();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4054Max_single.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
