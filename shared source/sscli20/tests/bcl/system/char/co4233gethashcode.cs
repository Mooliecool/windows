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
public class Co4233GetHashCode
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "GetHashCode  runTest() started." );
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   int iA = -2;
   int iB = -2;
   Char chr2 = '\0';
   iA = 0x0000;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_gh34");
   }
   iA = 0x0001;
   ++iCountTestcases;
   chr2 =  (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_j37");
   }
   iA = 0x0055;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode(); 
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_fh39");
   }
   iA = 0x007F;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_ie6");
   }
   iA = 0x0080;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_ahw7");
   }
   iA = 0x0082;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_qyw5");
   }
   iA = 0x0100;
   ++iCountTestcases;
   chr2 = (Char)iA;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_hsd82");
   }
   iA = 0x0123;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_jwqy6");
   }
   iA = 0x7F00;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_vcu7");
   }
   iA = 0x8000;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_lsj8");
   }
   iA = 0xA200;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_fh33");
   }
   iA = 0xA234;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_dj33");
   }
   iA = 0xFF00;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != 	GetHashCode(chr2)){
   ++iCountErrors;
   print("E_fh33");
   }
   iA = 0xFFFF;
   ++iCountTestcases;
   chr2 = (Char)iA ;
   iB = chr2.GetHashCode();
   if ( iB != GetHashCode(chr2) ){
   ++iCountErrors;
   print("E_skj2");
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Character\\Co4233GetHashCode.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4233GetHashCode.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4233GetHashCode.cs   FAiL !"  );
     return false;
     }
   }
 public virtual int GetHashCode(char value)
   {
   return   (int)value | ((int)value << 16);
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
   output.Append(" (Co4233GetHashCode.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co4233GetHashCode oCo4233GHCa = new Co4233GetHashCode();
   try
     {
     bResult = oCo4233GHCa.runTest();
     }
   catch (Exception exc)
     {
     bResult = false;
     Console.Error.WriteLine( "Character.GetHashCode: Co4233GHC FAiL." );
     Console.Error.WriteLine( exc.Message );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
