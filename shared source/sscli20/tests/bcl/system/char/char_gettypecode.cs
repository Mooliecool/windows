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
public class GetTypeCode
{
 public virtual bool runTest()
   {                
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int i2 = 0;
   char c2 = ' ';
   bool b2;
   Console.Error.WriteLine( "GetTypeCode runTest started." );
   ++iCountTestcases;
   i2 = 0x00; c2 = (char)i2;
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0x50; 
   if ( i2.GetTypeCode() == TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + i2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0x45; c2 = (char)i2; 
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0xFF; c2 = (char)i2; 
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0x30; c2 = (char)i2; 
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0x1234; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0x0000; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   ++iCountTestcases;
   i2 = 0xFFFF; c2 = (char)i2;
   b2 = Char.IsDigit( c2 );
   if ( c2.GetTypeCode() != TypeCode.Char )
     {
     ++iCountErrors;
     Console.WriteLine("Scenario :: " + iCountTestcases + "  GetTypeCode Failed... " + "Expected :: " + TypeCode.Char + "  Actual :: " + c2.GetTypeCode()); 
     }
   if ( iCountErrors == 0 ){
   Console.Error.Write( "GetTypeCode: paSs. iCountTestcases=={0}", iCountTestcases  );
   return true;
   }
   else{
   Console.Error.Write( "GetTypeCode: FAIL. iCountErrors=={0}",iCountErrors );
   return false;
   }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     GetTypeCode o2GetTypeCode = new GetTypeCode();
     bResult = o2GetTypeCode.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- GetTypeCode main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
