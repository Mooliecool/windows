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
public class Co1262Copy
{
 public int o_nState;
 public Co1262Copy()
   {o_nState = -1;}
 public Co1262Copy( int p_nState )
   {o_nState = p_nState;}
 public virtual bool runTest
   ()
   {
   System.Console.Error.WriteLine( "System.ArrayCopy: Co1262AC runTest running..." );
   int nErrorBits = 0; 
   char[] caCharArr2 = new char[33];
   char[] caCharArr3 = new char[ 6];
   Co1262Copy[] cbThisA = new Co1262Copy[6];
   Co1262Copy[] cbThisB = new Co1262Copy[6];
   caCharArr2[0] = 'm';
   caCharArr2[1] = 'n';
   caCharArr2[2] = 'o';
   caCharArr2[3] = 'p';
   caCharArr2[4] = 'q';
   caCharArr2[5] = 'r';
   caCharArr3[0] = 'C';
   caCharArr3[1] = 'D';
   caCharArr3[2] = 'E';
   caCharArr3[3] = 'F';
   caCharArr3[4] = 'G';
   caCharArr3[5] = 'H';
   Array.Copy(caCharArr2 ,2 ,caCharArr3 ,3 ,2); 
   if (
       caCharArr3[0] != 'C'
       ||  caCharArr3[1] != 'D'
       ||  caCharArr3[2] != 'E'
       ||  caCharArr3[3] != 'o'
       ||  caCharArr3[4] != 'p'
       ||  caCharArr3[5] != 'H'
       )
     nErrorBits |= 0x1;
   caCharArr2[0] = 'm';
   caCharArr2[1] = 'n';
   caCharArr2[2] = 'o';
   caCharArr2[3] = 'p';
   caCharArr2[4] = 'q';
   caCharArr2[5] = 'r';
   caCharArr3[0] = 'C';
   caCharArr3[1] = 'D';
   caCharArr3[2] = 'E';
   caCharArr3[3] = 'F';
   caCharArr3[4] = 'G';
   caCharArr3[5] = 'H';
   Console.WriteLine( "Loc_1284axc" );
   Array.Copy(caCharArr2 ,0 ,caCharArr3 ,0 ,caCharArr3.Length);
   if (
       caCharArr3[0] != 'm'
       ||  caCharArr3[1] != 'n'
       ||  caCharArr3[2] != 'o'
       ||  caCharArr3[3] != 'p'
       ||  caCharArr3[4] != 'q'
       ||  caCharArr3[5] != 'r'
       )
     nErrorBits |= 0x2;
   Console.WriteLine( "Loc_1772wiy" );
   for ( int aa=0 ;aa<cbThisA.Length ;aa++ )
     cbThisA[aa] = new Co1262Copy(aa + 100);
   for ( int aa=0 ;aa<cbThisB.Length ;aa++ )
     cbThisB[aa] = new Co1262Copy(aa + 2000);
   Array.Copy(cbThisA ,2 ,cbThisB ,3 ,2); 
   if (
       cbThisB[0].o_nState != 2000
       ||  cbThisB[1].o_nState != 2001
       ||  cbThisB[2].o_nState != 2002
       ||  cbThisB[3].o_nState !=  102
       ||  cbThisB[4].o_nState !=  103
       ||  cbThisB[5].o_nState != 2005
       )
     nErrorBits |= 0x1000;
   Console.WriteLine( "Loc_1505hkl" );
   for ( int aa=0 ;aa<cbThisA.Length ;aa++ )
     cbThisA[aa] = new Co1262Copy(aa + 100);
   for ( int aa=0 ;aa<cbThisB.Length ;aa++ )
     cbThisB[aa] = new Co1262Copy(aa + 2000);
   Array.Copy(cbThisA ,0 ,cbThisB ,0 ,cbThisB.Length);
   if (
       cbThisB[0].o_nState !=  100
       ||  cbThisB[1].o_nState !=  101
       ||  cbThisB[2].o_nState !=  102
       ||  cbThisB[3].o_nState !=  103
       ||  cbThisB[4].o_nState !=  104
       ||  cbThisB[5].o_nState !=  105
       )
     nErrorBits |= 0x2000;
   caCharArr2[0] = 'm';
   caCharArr2[1] = 'n';
   caCharArr2[2] = 'o';
   caCharArr2[3] = 'p';
   caCharArr2[4] = 'q';
   caCharArr2[5] = 'r';
   caCharArr3[0] = 'C';
   caCharArr3[1] = 'D';
   caCharArr3[2] = 'E';
   caCharArr3[3] = 'F';
   caCharArr3[4] = 'G';
   caCharArr3[5] = 'H';
   nErrorBits |= 0x4;
   Console.WriteLine( "Loc_1113vew" );
   try
     {
     Array.Copy(caCharArr2 ,0 ,caCharArr3 ,0 ,(caCharArr3.Length + 6) );  
     }
   catch (System.Exception exc)
     {
     nErrorBits &= (~0x4);
     }
   if (
       caCharArr3[0] == 'm'
       ||  caCharArr3[1] == 'n'
       ||  caCharArr3[2] == 'o'
       ||  caCharArr3[3] == 'p'
       ||  caCharArr3[4] == 'q'
       ||  caCharArr3[5] == 'r'
       )
     nErrorBits |= 0x8;
   Console.WriteLine( "Loc_1003bgt" );
   try
     {
     Array.Copy(caCharArr2 ,0 ,caCharArr3 ,0 ,0 );
     }
   catch (System.Exception exc)
     {
     nErrorBits |= 0x10;
     }
   if ( caCharArr3[0] != 'C' ) 
     nErrorBits |= 0x20;
   Console.WriteLine( "Loc_1990zqm" );
   nErrorBits |= 0x40;
   try
     {
     Array.Copy(caCharArr2 ,0 ,caCharArr3 ,0 ,-2 );
     }
   catch (System.Exception exc)
     {
     nErrorBits &= (~0x40);
     }
   if ( caCharArr3[0] != 'C' ) 
     nErrorBits |= 0x80;
   Console.WriteLine( "Loc_1652kjh" );
   nErrorBits |= 0x100;
   try
     {
     Array.Copy(caCharArr2 ,987 ,caCharArr3 ,0 ,1 );
     }
   catch (System.Exception exc)
     {
     nErrorBits &= (~0x100);
     }
   Console.WriteLine( "Loc_1336tuy" );
   nErrorBits |= 0x200;
   try
     {
     Array.Copy(caCharArr2 ,-1 ,caCharArr3 ,0 ,3 );
     }
   catch (System.Exception exc)
     {
     nErrorBits &= (~0x200);
     }
   if ( caCharArr3[0] != 'C' ) 
     nErrorBits |= 0x400;
   Console.WriteLine( "Loc_1444cin" );
   if ( nErrorBits == 0 ) {
   return true; 
   }
   else
     {
     System.Console.Error.Write( "System- Co1262Copy nErrorBits==" );
     System.Console.Error.WriteLine(  nErrorBits.ToString() );
     return false; 
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co1262Copy oCo1262ACa = new Co1262Copy();
   bResult = oCo1262ACa.runTest
     ();
   if (bResult == true) System.Console.Error.WriteLine( "System.ArrayCopy: Co1262AC paSs." );
   else                 System.Console.Error.WriteLine( "System.ArrayCopy: Co1262AC FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
