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
public class Co1078Clear
{
 public virtual bool runTest()
   {
   System.Console.WriteLine( "System.Array.Clear Co1078AC runTest started." );
   int nErrorBits = 0; 
   int nA = -2;
   int nB = -2;
   int[] iIntArr2 = new int[9];
   System.Int32[] intInteger4Arr3 = new System.Int32[9];
   Int32[] iArrA;
   Int32[] iArrB;
   Int32
     bb = -1
     ,iTemp;
   Array aArrG;
   String sLoc="Loc_nut100";
   try
     { 
     iArrA = new Int32[] { 22 };
     iArrB = new Int32[] {  1 };
     sLoc="Loc_buv201";
     aArrG = Array.CreateInstance( typeof(Int32) ,iArrA ,iArrB );
     sLoc="Loc_buv211";
     Console.WriteLine();
     for ( bb=1 ;bb<(1 + aArrG.GetLength( 0 )) ;bb++ )
       {
       sLoc="Loc_buv221";
       aArrG.SetValue( (100 + bb) ,bb );
       sLoc="Loc_buv231";
       sLoc="Loc_buv241";
       if ( (Int32)(aArrG.GetValue( bb )) != (100 + bb) )
	 {
	 nErrorBits |= (Int32)0x200;
	 Console.WriteLine( "Error_rad212:  Improper value in array.  bb="+ bb.ToString() );
	 }
       sLoc="Loc_buv244";
       }
     Console.WriteLine();
     Console.WriteLine( "--------  Info_wef553  ------" );
     Console.WriteLine();
     Array.Clear( aArrG ,aArrG.GetLowerBound(0) ,aArrG.GetLength(aArrG.Rank - 1) );
     sLoc="Loc_buv251";
     for ( bb=1 ;bb<(1 + aArrG.GetLength( 0 )) ;bb++ )
       {
       sLoc="Loc_buv261";
       sLoc="Loc_buv271";
       iTemp = (Int32)(aArrG.GetValue( bb ));
       if ( iTemp != 0 )  
	 {
	 nErrorBits |= (Int32)0x300;
	 Console.WriteLine( "Error_rad362:  Improper value in array.  iTemp="+ iTemp.ToString() +" ,bb="+ bb.ToString() );
	 }
       sLoc="Loc_buv244";
       }
     Console.WriteLine();
     Console.WriteLine( "--------  Info_wef822  ------" );
     Console.WriteLine();
     }
   catch ( Exception exc123 )
     {
     nErrorBits |= (Int32)0x100;
     Console.WriteLine( "Error_rad527:  UNexpected exception near sLoc="+ sLoc +", bb="+ bb.ToString() +", exc123= ..." );
     Console.WriteLine( exc123.ToString() );
     }
   sLoc="Loc_buv281";
   for (int aa=0 ;aa<iIntArr2.Length ;aa++)
     {
     iIntArr2[aa] = aa + 100;
     }
   Array.Clear( iIntArr2 ,0 ,iIntArr2.Length );
   for (int aa=0 ;aa<iIntArr2.Length ;aa++)
     {
     if ( iIntArr2[aa] != 0 )
       {
       nErrorBits |= 0x1;
       }
     }
   nA = 2; 
   nB = 2; 
   for (int aa=0 ;aa<iIntArr2.Length ;aa++)
     {
     iIntArr2[aa] = aa + 200;
     }
   Array.Clear( iIntArr2 ,nA ,nB );
   for (int aa=0 ;aa<iIntArr2.Length ;aa++)
     {
     if ( (aa < nA) || (aa >= (nB+nA)) )
       {
       if ( iIntArr2[aa] == 0 )
	 nErrorBits |= 0x2;
       }
     else
       {
       if ( iIntArr2[aa] != 0 )
	 nErrorBits |= 0x4;
       }
     }
   for (int aa=0 ;aa<intInteger4Arr3.Length ;aa++)
     {
     intInteger4Arr3[aa] = aa + 1000;
     }
   Array.Clear( intInteger4Arr3 ,0 ,intInteger4Arr3.Length );
   for (int aa=0 ;aa<intInteger4Arr3.Length ;aa++)
     {
     if ( intInteger4Arr3[aa] != 0 )
       nErrorBits |= 0x10;
     }
   nA = 2; 
   nB = 2; 
   for (int aa=0 ;aa<intInteger4Arr3.Length ;aa++)
     {
     intInteger4Arr3[aa] = (aa + 2000);
     }
   Array.Clear( intInteger4Arr3 ,nA ,nB );
   for (int aa=0 ;aa<(nB+nA) ;aa++)
     {
     if ( (aa < nA) || (aa >= (nB+nA)) )
       {
       if ( intInteger4Arr3[aa] == 0 )
	 nErrorBits |= 0x20;
       }
     else
       {
       if ( intInteger4Arr3[aa] != 0 )
	 nErrorBits |= 0x40;
       }
     }
   int[][][] i4Arr = new int[3][][];
   i4Arr[0] = new int[1][];
   i4Arr[1] = new int[2][];
   i4Arr[2] = new int[3][];
   i4Arr[0][0] = new int[] {1,1,1};
   i4Arr[1][0] = new int[] {1,1,1,1};
   i4Arr[1][1] = new int[] {1,1,1,1,1};
   i4Arr[2][0] = new int[] {};
   i4Arr[2][1] = new int[] {1};
   i4Arr[2][2] = new int[1000];
   Array.Clear(i4Arr, 0, 3 );
   for( int i=0; i< 3; i++)
     {
     if( i4Arr[i] != null )
       {
       nErrorBits |= 0x80;
       Console.WriteLine( "Array.Clear failed on jagged array."  );
       }
     }
   int[,] a2 = new int[,] {{1, 2, 3}, {4, 5, 6}};
   Array.Clear( a2, 2, 3 );
   if( a2[0,2] != 0 || a2[1,0] != 0 || a2[1,1] != 0)
     {
     nErrorBits |= 0xF0;
     Console.WriteLine( "Array.Clear failed to clear required elements."  );
     }
   if( a2[0,0] == 0 || a2[0,1] == 0 || a2[1,2] == 0)
     {
     nErrorBits |= 0x100;
     Console.WriteLine( "Array.Clear cleared the elements that is not in clear range.");
     }
   System.Console.Write( "System.Array.Clear Co1078AC nErrorBits==" );
   System.Console.WriteLine( nErrorBits );
   if ( nErrorBits == 0 ) {
   return true;  
   }
   else  {
   return false; 
   }
   }
 public static void Main( String[] args )
   {
   System.Console.WriteLine( "System.Array.Clear Co1078AC main started." );
   bool bResult = false; 
   Co1078Clear oCo1078ACa = new Co1078Clear();
   try
     {
     bResult = oCo1078ACa.runTest();
     }
   catch (System.Exception exc)
     {
     bResult = false;
     System.Console.WriteLine( exc.Message );
     }
   if (bResult == true) System.Console.WriteLine( "System.Array.Clear Co1078AC paSs." );
   else                 System.Console.WriteLine( "System.Array.Clear Co1078AC FAiL." );
   if (bResult == true) Environment.ExitCode = 0;
   else                 Environment.ExitCode = 11;
   }
} 
