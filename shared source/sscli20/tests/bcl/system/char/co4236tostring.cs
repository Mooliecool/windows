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
public class Co4236ToString
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co4236ToString	runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   char c2 = ' ';
   String str2 = null;
   String str9 = null;
   c2 = 'a';
   str9 =  "a" ;
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_uycn");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_oi38");
   }
   c2 = 'A';
   str9 =  "A" ;
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_do2");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_aj2");
   }
   c2 = (char)0x0000;
   str9 =  c2.ToString();
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_uycn");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_oi38");
   }
   c2 = (char)0x000A;
   str9 = c2.ToString();
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_j28");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_kas");
   }
   c2 = (char)0x000D;
   str9 = c2.ToString();
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_oiw8");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_eiub");
   }
   c2 = ' ';
   str9 = c2.ToString();
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_doi3");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_vri2");
   }
   c2 = '\0';
   str9 = c2.ToString();
   str2 = Char.ToString(c2);
   ++iCountTestcases;
   if ( ! str2.Equals( str9 ) ){
   ++iCountErrors;
   print("E_d2i1");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_1o33");
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Co4236ToString: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4236ToString iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Co4236ToString: FAiL!" );
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
   output.Append(" (Co4236ToString.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4236ToString o2Co4236ToString = new Co4236ToString();
     bResult = o2Co4236ToString.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4236ToString main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
