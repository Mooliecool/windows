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
public class Co4235ToString
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co4235ToString	runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   char c2 = ' ';
   char c3 = ' ';
   String str2 = null;
   String str9 = null;
   Char char2 = '\0';
   c2 = 'a';
   str9 =  "a" ;
   char2 =  c2 ;
   str2 = char2.ToString();
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
   char2 =  c2 ;
   str2 = char2.ToString();
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
   char2 =  c2 ;
   str2 = char2.ToString();
   c3 = str2.Substring( 0 ,1 )[0];
   ++iCountTestcases;
   if ( ! (c2 == c3) ){
   ++iCountErrors;
   print("E_d2p1");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_d2j2");
   }
   c2 = (char)0x000A;
   char2 =  c2 ;
   str2 = char2.ToString();
   c3 = str2.Substring( 0 ,1 )[0];
   ++iCountTestcases;
   if ( ! (c2 == c3) ){
   ++iCountErrors;
   print("E_asl2");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_dk29");
   }
   c2 = (char)0x000D;
   char2 =  c2 ;
   str2 = char2.ToString();
   c3 = str2.Substring( 0 ,1 )[0];
   ++iCountTestcases;
   if ( ! (c2 == c3) ){
   ++iCountErrors;
   print("E_da8");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_ag71");
   }
   c2 = ' ';
   char2 =  c2 ;
   str2 = char2.ToString();
   c3 = str2.Substring( 0 ,1 )[0];
   ++iCountTestcases;
   if ( ! (c2 == c3) ){
   ++iCountErrors;
   print("E_zj28");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_al9");
   }
   c2 = '\0';
   char2 =  c2 ;
   str2 = char2.ToString();
   c3 = str2.Substring( 0 ,1 )[0];
   ++iCountTestcases;
   if ( ! (c2 == c3) ){
   ++iCountErrors;
   print("E_os9");
   }
   ++iCountTestcases;
   if ( ! (str2.Length == 1) ){
   ++iCountErrors;
   print("E_h28");
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Co4235ToString: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4235ToString iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Co4235ToString: FAiL!" );
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
   output.Append(" (Cb4201Put.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   try
     {
     Co4235ToString o2Co4235ToString = new Co4235ToString();
     bResult = o2Co4235ToString.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine( "Character- Co4235ToString main caught Exception!" );
     Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
