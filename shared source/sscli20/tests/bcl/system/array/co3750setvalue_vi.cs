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
using System.Threading;
using System.Reflection;
public class Co3750SetValue_vi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.SetValue(Object, int)";
 public static String s_strTFName        = "Co3750SetValue_vi";
 public static String s_strTFAbbrev      = "Co3750";
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int[] in4Arr1Orig;
   double[] doArr = { 0.1 ,1.2 ,2,3 };
   Object[] varArr1Orig;
   Int16[] in2Arr;
   Int32[] in4Arr;
   Int64[] in8Arr;
   Single[] sglArr;
   Double[] dblArr;
   Byte[] bytArr;
   SByte[] sbytArr;
   Decimal[] currArr;
   Decimal[] decArr;
   Object[] varArr;
   Object[] objArr;
   int in4Before;
   int in4After;
   Object varNewValue;
   Object obj1=null;
   try
     {
     do
       {
       strLoc="Loc_110dt";
       in4Arr1Orig = new int[3];
       for ( int ia = 0 ;ia < in4Arr1Orig.Length ;ia++ )
	 {
	 in4Arr1Orig[ia] = ia;
	 }
       in4Before = in4Arr1Orig[0];
       in4After = in4Arr1Orig.Length;
       varNewValue = in4After;
       in4Arr1Orig.SetValue( (Object)  varNewValue, 0 );
       ++iCountTestcases;
       if ( in4After != in4Arr1Orig[0] )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_579yr!  in4After=="+ in4After +" ,in4Arr1Orig[0]=="+ in4Arr1Orig[0] );
	 }
       strLoc="Loc_132ke";
       in4Arr1Orig = new int[3];
       for ( int ia = 0 ;ia < in4Arr1Orig.Length ;ia++ )
	 {
	 in4Arr1Orig[ia] = ia;
	 }
       in4Before = in4Arr1Orig[0];
       in4After = in4Arr1Orig.Length;
       varNewValue = (int)in4After;
       in4Arr1Orig.SetValue( (Object) (int)varNewValue, 0  );
       ++iCountTestcases;
       if ( in4After != in4Arr1Orig[0] )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_763bw!  in4After=="+ in4After +" ,in4Arr1Orig[0]=="+ in4Arr1Orig[0] );
	 }
       strLoc="Loc_110dt";
       doArr = new double[3];
       doArr[0] = 0.1;
       doArr[1] = 1.2;
       doArr[2] = 2.3;
       doArr.SetValue( (Object)  ( 3.4 ), 0  );
       ++iCountTestcases;
       if ( doArr[0] != 3.4 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_397xa!  doArr[0]=="+ doArr[0] );
	 }
       strLoc = "Loc_485eu";
       in2Arr = new Int16[2];
       iCountTestcases++;
       try {
       in2Arr.SetValue( (Object) 4, -1);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_5287a");
       } catch (IndexOutOfRangeException) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_5278q, " + exc.ToString());
       }
       iCountTestcases++;
       try {
       in2Arr.SetValue( (Object) 4, 2);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_523ua");
       } catch (IndexOutOfRangeException) {}
       catch ( Exception exc ) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_234ay, " + exc.ToString());
       }
       in2Arr = new Int16[2];
       in2Arr.SetValue( (Object) (short)2, 0);
       in2Arr.SetValue( (Object)  (short)(-2), 1);
       iCountTestcases++;
       if(in2Arr[0] != 2 || in2Arr[1] != -2)
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_3928u");
	 }
       in2Arr = new Int16[2];
       try {
       in2Arr.SetValue( (Object) 2, 0);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_234uq");
       } catch (ArgumentException) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_383da! , exc=="+exc);
       }
       strLoc = "Loc_485ef";
       in4Arr = new Int32[2];
       iCountTestcases++;
       try {
       in4Arr.SetValue( (Object) 4, -1);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_52ssa");
       } catch (IndexOutOfRangeException) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_5278q, " + exc.ToString());
       }
       iCountTestcases++;
       try {
       in4Arr.SetValue( (Object) (Object)4, 2);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_52vua");
       } catch (IndexOutOfRangeException) {}
       catch ( Exception exc ) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_234ay, " + exc.ToString());
       }
       strLoc = "Loc_373uy";
       in4Arr = new Int32[2];
       in4Arr.SetValue((Object)((short)2), 0 );
       in4Arr.SetValue((Object)(int) -20, 1);
       iCountTestcases++;
       if((Int32) in4Arr.GetValue(0) != 2 || in4Arr[1] != -20)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2349w");
	 }
       strLoc = "Loc_486ef";
       in8Arr = new Int64[2];
       iCountTestcases++;
       try {
       in8Arr.SetValue((Object)4, -1);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_5234a");
       } catch (IndexOutOfRangeException) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_5228q, " + exc.ToString());
       }
       iCountTestcases++;
       try {
       in8Arr.SetValue((Object)4, 2);
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_524ua");
       } catch (IndexOutOfRangeException) {}
       catch ( Exception exc ) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_233ay, " + exc.ToString());
       }
       strLoc = "Loc_373fg";
       in8Arr = new Int64[3];
       in8Arr.SetValue((Object)((short)2), 0);
       in8Arr.SetValue((Object)((int)-20), 1);
       in8Arr.SetValue((Object)((long)200), 2);
       iCountTestcases++;
       if((Int64)in8Arr.GetValue(0) != (long)2 || in8Arr[1] != (long)(-20) || (Int64)in8Arr.GetValue(2) != (long)200)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2344w!, in8Arr[0]=="+(Int64)in8Arr.GetValue(0)+", in8Arr[1]=="+(Int64)in8Arr.GetValue(1)+", in8Arr[2]=="+(Int64)in8Arr.GetValue(2));
	 }
       strLoc = "Loc_438iw";
       sglArr = new Single[2];
       sglArr.SetValue((Object)((Single)5.5), 0);
       sglArr.SetValue((Object)((Single)(-5.5)), 1);
       iCountTestcases++;
       if((Single) sglArr.GetValue(0) != (Single)5.5 || sglArr[1] != (Single)(-5.5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_572ss");
	 }
       strLoc = "Loc_4fw3w";
       dblArr = new Double[2];
       dblArr.SetValue((Object)(Double)5.5, 0);
       strLoc = "Loc_4fw3w.2";
       dblArr.SetValue((Object)(Double)(-5.5), 1);
       iCountTestcases++;
       strLoc = "Loc_4fw3w.3";
       if( ((Double) dblArr.GetValue(0)) != 5.5 || dblArr[1] != -5.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_5we3s");
	 }
       strLoc = "Loc_982cu";
       bytArr = new Byte[2];
       bytArr.SetValue((Object)((Byte)23), 0);
       bytArr.SetValue((Object)((Byte)5), 1);
       iCountTestcases++;
       if(bytArr[0] != (Byte)23 || (Byte) bytArr.GetValue(1) != (Byte)5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_2389q! ,bytArr[0]=="+(Byte) bytArr.GetValue(0)+", bytArr[1]=="+bytArr.GetValue(1));
	 }
       strLoc = "Loc_492te";
       sbytArr = new SByte[2];
       sbytArr.SetValue((Object)((sbyte)-3), 0);
       sbytArr.SetValue((Object)((sbyte)2), 1);
       iCountTestcases++;
       if(sbytArr[0] != (sbyte)(-3) || (sbyte) sbytArr.GetValue(1) != (sbyte)2)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_089iq");
	 }
       strLoc = "Loc_987wu";
       currArr = new Decimal[2];
       try {
       currArr.SetValue((Object)((Decimal)4), 0);
       currArr.SetValue((Object)((Decimal)5.5), 1);
       iCountTestcases++;
       if(currArr[0] != (Decimal)4 || (Decimal) currArr.GetValue(1) != (Decimal)5.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_472su");
	 }
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_347qw! , exc=="+exc);
       }
       try {
       strLoc = "Loc_3874i";
       decArr = new Decimal[2];
       decArr.SetValue((Object)((Decimal)(-3.4)), 0);
       decArr.SetValue((Object)((Decimal)5.5), 1);
       iCountTestcases++;
       if(decArr[0] != (Decimal)(-3.4) || (Decimal) decArr.GetValue(1) != (Decimal)5.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_289iw!, decArr[0]=="+(Decimal) decArr.GetValue(0)+", decArr[1]=="+decArr.GetValue(1));
	 }
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_394jd! , exc=="+exc);
       }
       strLoc = "Loc_9384w";
       varArr = new Object [10];
       try {
       varArr.SetValue((Object)unchecked ((byte)(-5)), 0);
       varArr.SetValue((Object)(Byte)5, 1);
       varArr.SetValue((Object)(short)10, 2);
       varArr.SetValue((Object)15, 3);
       varArr.SetValue((Object)(long)20, 4);
       varArr.SetValue((Object)(float)5.5, 5);
       varArr.SetValue((Object)10.1, 6);
       varArr.SetValue((Object)(Decimal)13.5, 7);
       varArr.SetValue((Object)(Decimal)(-1.4), 8);
       varArr.SetValue((Object)(Object)5, 9);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_893ya! , exc=="+exc);
       }
       strLoc = "Loc_384uw";
       iCountTestcases++;
       if((byte)varArr.GetValue(0) != unchecked ((byte)(-5)))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_983sq");
	 }
       strLoc = "Loc_0983w";
       iCountTestcases++;
       if((Byte)varArr.GetValue(1) != (Byte)5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_983iu");
	 }
       strLoc = "Loc_984uw";
       iCountTestcases++;
       if((short)varArr.GetValue(2) != (short)10)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_984uq");
	 }
       strLoc = "Loc_12sqq";
       iCountTestcases++;
       if((int) varArr.GetValue(3) != 15)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_482oi");
	 }
       strLoc = "Loc_894yr";
       iCountTestcases++;
       if((long)varArr.GetValue(4) != (long)20)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_148cs");
	 }
       strLoc = "Loc_857ye";
       iCountTestcases++;
       if((float)varArr.GetValue(5) != (float)5.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_9557e");
	 }
       strLoc = "Loc_573qq";
       iCountTestcases++;
       if((double) varArr.GetValue(6) != (double) 10.1)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_1238f");
	 }
       strLoc = "Loc_5984e";
       iCountTestcases++;
       if((Decimal) varArr.GetValue(7) != (Decimal)13.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_5983u");
	 }
       strLoc = "Loc_585jk";
       iCountTestcases++;
       if((Decimal)varArr.GetValue(8) != (Decimal)(-1.4))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_853ad");
	 }
       strLoc = "Loc_7355y";
       iCountTestcases++;
       if(!varArr.GetValue(9).Equals ((Object)5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_23189");
	 }
       strLoc = "Loc_766qm";
       objArr = new Object[15];
       try {
       objArr.SetValue( (Object) unchecked ((byte)(-5)), 0);
       objArr.SetValue( (Object) (Byte)5, 1);
       objArr.SetValue( (Object) (short)10, 2);
       objArr.SetValue( (Object) 15, 3);
       objArr.SetValue( (Object) (long)20, 4);
       objArr.SetValue( (Object) (float)5.5, 5);
       objArr.SetValue( (Object) 10.1, 6);
       objArr.SetValue( (Object) (Decimal)13.5, 7);
       objArr.SetValue( (Object) (Decimal)(-1.4), 8);
       objArr.SetValue( (Object)5, 9);
       objArr.SetValue( (Object) (Boolean)true, 10);
       objArr.SetValue( (Object) (Char)'a', 11);
       objArr.SetValue( (Object) new DateTime(1999,3,22), 12);
       obj1 = new Co3750SetValue_vi();
       objArr.SetValue( (Object) obj1, 13);
       objArr.SetValue( (Object) TimeSpan.MinValue, 14 );
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_764an! , exc=="+exc);
       }
       strLoc = "Loc_872dr";
       iCountTestcases++;
       if((byte)objArr.GetValue(0) != unchecked ((byte)(-5)))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev +  " Err_106xs objArr.GetValue(0)=<" + objArr.GetValue(0) + ">");
	 }
       strLoc = "Loc_873sd";
       iCountTestcases++;
       if((Byte)objArr.GetValue(1) != (Byte)5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_106ze");
	 }
       strLoc = "Loc_106za";
       iCountTestcases++;
       if((short)objArr.GetValue(2) != (short)10)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_0003fg");
	 }
       strLoc = "Loc_167zm";
       iCountTestcases++;
       if((int) objArr.GetValue(3) != 15)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_073gd");
	 }
       strLoc = "Loc_062xe";
       iCountTestcases++;
       if((long)objArr.GetValue(4) != (long)20)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_183sA");
	 }
       strLoc = "Loc_426gs";
       iCountTestcases++;
       if((float)objArr.GetValue(5) != (float)5.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_634sn");
	 }
       strLoc = "Loc_632xe";
       iCountTestcases++;
       if((double) objArr.GetValue(6) != (double)10.1)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_642sw");
	 }
       strLoc = "Loc_106pn";
       iCountTestcases++;
       if((Decimal)objArr.GetValue(7) != (Decimal)13.5)
	 {
	 iCountErrors++;
	 Console.WriteLine( "Err_107am, objArr.GetValue(7) == " + (Decimal) objArr.GetValue(7));
	 }
       strLoc = "Loc_105wm";
       iCountTestcases++;
       if((Decimal)objArr.GetValue(8) != (Decimal)(-1.4))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_102xk");
	 }
       strLoc = "Loc_295xf";
       iCountTestcases++;
       if(!objArr.GetValue(9).Equals ((Object)5))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_848xl");
	 }
       strLoc = "Loc_351sa";
       iCountTestcases++;
       if(!objArr.GetValue(10).Equals ((Object) true))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_537aw - " + objArr.GetValue(10));
	 }
       strLoc = "Loc_573aw";
       iCountTestcases++;
       if(!objArr.GetValue(11).Equals ((Object) 'a'))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_532em - " + objArr.GetValue(11));
	 }
       strLoc = "Loc_073aq";
       iCountTestcases++;
       if((DateTime) objArr.GetValue(12) != new DateTime(1999,3,22))
	 {
	 iCountErrors++;
	 Console.WriteLine( "Err_023al, objArr.GetValue(12) == " + objArr.GetValue(12));
	 }
       strLoc = "Loc_156aw";
       iCountTestcases++;
       if(objArr.GetValue(13) != obj1)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_264vl objArr.GetValue(13)== " + objArr.GetValue(13));
	 }
       strLoc = "Loc_752vd";
       iCountTestcases++;
       if((TimeSpan) objArr.GetValue(14) != TimeSpan.MinValue)
	 {
	 iCountErrors++;
	 Console.WriteLine( "Err_426st, objArr.GetValue(14) == " + objArr.GetValue(14));
	 }
       varArr1Orig = new Object [2];
       varArr1Orig.SetValue( (Object) System.DBNull.Value, 0);
       varArr1Orig.SetValue( (Object) System.DBNull.Value, 1);
       iCountTestcases++;
       if((System.DBNull) varArr1Orig.GetValue(0) != System.DBNull.Value)
	 {
	 iCountErrors++;
	 Console.WriteLine( "Err_578er, varArr1Orig.GetValue(0) == " + varArr1Orig.GetValue(0));
	 }
       iCountTestcases++;
       if(!varArr1Orig.GetValue(1).Equals(System.DBNull.Value)) 
	 {
	 iCountErrors++;
	 Console.WriteLine( "Err_740dw, varArr1Orig.GetValue(1) == " + varArr1Orig.GetValue(1));
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   } 
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co3750SetValue_vi cbA = new Co3750SetValue_vi();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
} 
