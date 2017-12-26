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
public class Co4234Equals
{
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co4234Equals.cs  runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Char charVal1 = '\0';
   Char charRes1 = '\0';
   do
     {
     charVal1 = 'A' ;
     charRes1 = 'A' ;
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_dj33");
     }
     charVal1 = 'Z' ;
     charRes1 = 'Z' ;
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_h388");
     }
     charVal1 = 'a';
     charRes1 = 'a';
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_khj38");
     }
     charVal1 =  'z' ;
     charRes1 =  'z' ;
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_dk38");
     }
     charVal1 =  '@' ;
     charRes1 = '@' ;
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_ak38");
     }
     charVal1 =  '[' ;
     charRes1 = '[' ;
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_ahw7");
     }
     charVal1 =  ' ' ;
     charRes1 = ' ';
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_fjw9");
     }
     charVal1 =  '\0' ;
     charRes1 = '\0';
     ++iCountTestcases;
     if ( ! charVal1.Equals( charRes1 ) ) {
     ++iCountErrors;
     print("E_djA9");
     }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Character\\Co4234Equals.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4234Equals.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4234Equals.cs   FAiL !"  );
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Error.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co4234Equals.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4234Equals cbA = new Co4234Equals();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4234Equals.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
