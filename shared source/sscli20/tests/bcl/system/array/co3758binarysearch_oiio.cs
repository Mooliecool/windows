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
public class Co3758BinarySearch_oiio
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Array.BinarySearch(Object[], int Index, int Count, Object)";
 public static readonly String s_strTFName        = "Co3758BinarySearch_oiio";
 public static readonly String s_strTFAbbrev      = "Co3758";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="Loc_top100";
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Object o1;
   int iReturnValue;
   Object[] oArrValues;
   int iCount;
   try
     {
     do
       {
       ++iCountTestcases;  
       if ( ! TestCaseRoutine() )
	 {
	 ++iCountErrors;
	 }
       strLoc="Loc_heg544";
       oArrValues = new Object[M_cArr.Length];
       for(int i=0; i < M_cArr.Length; i++)
	 {
	 oArrValues[i]=M_cArr[i];
	 }
       strLoc="Loc_hep344";
       Array.Sort(oArrValues);
       iCount=6;
       ++iCountTestcases;
       for(int i=2; i < 8; i++)
	 {
	 strLoc="458sz_"+i;
	 o1 = oArrValues[i];
	 iReturnValue = Array.BinarySearch(oArrValues, i, iCount, o1);
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine(iCount);
	   Console.WriteLine(o1);
	   Console.WriteLine( s_strTFAbbrev + "Err_320ye_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 iCount--;
	 }
       strLoc="Loc_huk506";
       oArrValues = new Object[M_i16Arr.Length];
       for(int i=0; i < M_i16Arr.Length; i++)
	 {
	 oArrValues[i]=M_i16Arr[i];
	 }
       Array.Sort(oArrValues);
       strLoc="375yr";
       iCount=6;
       ++iCountTestcases;
       for(int i=2; i < 8; i++)
	 {
	 strLoc="863gq_"+i;
	 o1 = oArrValues[i];
	 iReturnValue = Array.BinarySearch(oArrValues, i, iCount, o1);
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_947ps_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 iCount--;
	 }
       strLoc="Loc_hum806";
       ++iCountTestcases;
       try
	 {
	 o1 = 1;
	 iReturnValue = Array.BinarySearch(null, 0, 2, o1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException )
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_972qr!  , Wrong Exception thrown == " + ex.ToString() );
	 }
       strLoc="Loc_hus706";
       oArrValues = new Object[M_i16Arr.Length];
       for(int i=0; i < M_i16Arr.Length; i++)
	 {
	 oArrValues[i]=M_i16Arr[i];
	 }
       try
	 {
	 o1 = 1;
	 ++iCountTestcases;
	 iReturnValue = Array.BinarySearch(oArrValues, -2, 2, o1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_407hr!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException )
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_024df!  , Wrong Exception thrown == " + ex.ToString() );
	 }
       strLoc="Loc_huz906";
       try
	 {
	 o1 = 1;
	 ++iCountTestcases;
	 iReturnValue = Array.BinarySearch(oArrValues, 2, -2, o1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_948ds!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException )
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_902sa!  , Wrong Exception thrown == " + ex.ToString() );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev + "Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   } 
 public Boolean TestCaseRoutine()  
   {
   Boolean bToReturn = true;  
   String sLoc="Loc_gub200";
   Array aAsArray;
   Int32
     aa
     ,iCountCatches = 0
     ,iFoundIdx
     ,iSourceIdx;
   short[] i16Arr = { Int16.MinValue ,-12345 ,-2 ,-1,-1 ,0 ,1 ,2 ,12345 ,Int16.MaxValue };
   short[] i16DescArr = { Int16.MaxValue ,12345 ,2 ,1,1 ,0 ,-1 ,-2 ,-12345 ,Int16.MinValue };
   int[] i32Arr = { Int32.MinValue ,-12345 ,-2 ,-1 ,0 ,1 ,2 ,12345 ,Int32.MaxValue} ;
   long[] i64Arr = { Int64.MinValue ,Int64.MinValue ,-654321,-12345 ,-2 ,-1 ,0 ,1 ,2 ,12345 ,654321 ,Int64.MaxValue ,Int64.MaxValue };
   double[] f8Arr1 = { Double.NegativeInfinity ,Double.MinValue ,-Double.Epsilon ,(double)0.0 ,(double)0.0 ,Double.Epsilon ,Double.MaxValue ,Double.PositiveInfinity };
   double[] f8Arr2 = { Double.NaN };
   double[] f8Arr3 = { };
   UIntPtr[] uipArr = { new UIntPtr((UInt32)0) ,new UIntPtr((UInt32)654321) ,new UIntPtr((UInt32)654321) ,new UIntPtr((UInt32)987654321) };
   sLoc="Loc_gub210";
   try
     { 
     aAsArray = Array.CreateInstance( typeof( short ) ,i16Arr.Length );
     for ( aa=0 ;aa<i16Arr.Length ;aa++ )
       {
       aAsArray.SetValue( (short)i16Arr[aa] ,aa );
       }
     sLoc="Loc_gub220";
     for ( aa=0 ;aa<i16Arr.Length ;aa++ )
       {
       iSourceIdx = aa;
       iFoundIdx = Array.BinarySearch
	 (
	  i16Arr
	  ,0 ,i16Arr.Length
	  ,(short)i16Arr[iSourceIdx] );
       sLoc="Loc_gub230";
       if ( (short)i16Arr[iFoundIdx] != (short)i16Arr[iSourceIdx] )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_gub321:  i16Arr ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       sLoc="Loc_gub240";
       iFoundIdx = Array.BinarySearch
	 (
	  aAsArray
	  ,0 ,aAsArray.Length
	  ,(short)( aAsArray.GetValue( iSourceIdx ) )
	  );
       sLoc="Loc_gub250";
       if ( (short)aAsArray.GetValue(iFoundIdx) != (short)aAsArray.GetValue(iSourceIdx) )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_gub456:  aAsArray-i16Arr ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       sLoc="Loc_gub260";
       }
     aAsArray = Array.CreateInstance( typeof( short ) ,i16DescArr.Length );
     for ( aa=0 ;aa<i16DescArr.Length ;aa++ )
       {
       aAsArray.SetValue( (short)i16DescArr[aa] ,aa );
       }
     iCountCatches = 0;
     for ( aa=0 ;aa<i16DescArr.Length ;aa++ )
       {
       iSourceIdx = aa;
       try
	 {
	 iFoundIdx = Array.BinarySearch
	   (
	    i16DescArr
	    ,0 ,i16DescArr.Length
	    ,(short)i16DescArr[iSourceIdx] );
	 if ( (short)i16DescArr[iFoundIdx] != (short)i16DescArr[iSourceIdx] )
	   {
	   bToReturn = false;
	   Console.WriteLine( "Error_sot321:  i16DescArr ,"
			      + "iSourceIdx=="
			      + iSourceIdx.ToString() +" ,iFoundIdx=="
			      + iFoundIdx.ToString() );
	   }
	 }catch( IndexOutOfRangeException  )
	   { ++iCountCatches; }
       sLoc="Loc_sot310";
       try
	 {
	 iFoundIdx = Array.BinarySearch
	   (
	    aAsArray
	    ,0 ,aAsArray.Length
	    ,(short)( aAsArray.GetValue( iSourceIdx ) )
	    );
	 if ( (short)aAsArray.GetValue(iFoundIdx) != (short)aAsArray.GetValue(iSourceIdx) )
	   {
	   bToReturn = false;
	   Console.WriteLine( "Error_sot456:  aAsArray-i16DescArr ,"
			      + "iSourceIdx=="
			      + iSourceIdx.ToString() +" ,iFoundIdx=="
			      + iFoundIdx.ToString() );
	   }
	 }catch( IndexOutOfRangeException  )
	   { ++iCountCatches; }
       }
     if ( iCountCatches <= 0 )
       {
       bToReturn = false;
       Console.WriteLine( "Error_sot436:  Expected exception not thrown, for Desc sort?" );
       }
     aAsArray = Array.CreateInstance( typeof( long ) ,i64Arr.Length );
     for ( aa=0 ;aa<i64Arr.Length ;aa++ )
       {
       aAsArray.SetValue( (long)i64Arr[aa] ,aa );
       }
     for ( aa=0 ;aa<i64Arr.Length ;aa++ )
       {
       iSourceIdx = aa;
       iFoundIdx = Array.BinarySearch
	 (
	  i64Arr
	  ,0 ,i64Arr.Length
	  ,(long)i64Arr[iSourceIdx] );
       if ( (long)i64Arr[iFoundIdx] != (long)i64Arr[iSourceIdx] )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_zek321:  i64Arr ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       sLoc="Loc_zek310";
       iFoundIdx = Array.BinarySearch
	 (
	  aAsArray
	  ,0 ,aAsArray.Length
	  ,(long)( aAsArray.GetValue( iSourceIdx ) )
	  );
       if ( (long)aAsArray.GetValue(iFoundIdx) != (long)aAsArray.GetValue(iSourceIdx) )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_zek456:  aAsArray-i64Arr ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       }
     aAsArray = Array.CreateInstance( typeof( UIntPtr ) ,uipArr.Length );
     for ( aa=0 ;aa<uipArr.Length ;aa++ )
       {
       aAsArray.SetValue( (UIntPtr)uipArr[aa] ,aa );
       }
     sLoc="Loc_pit230";
     aAsArray = Array.CreateInstance( typeof( double ) ,f8Arr1.Length );
     for ( aa=0 ;aa<f8Arr1.Length ;aa++ )
       {
       aAsArray.SetValue( (double)f8Arr1[aa] ,aa );
       }
     for ( aa=0 ;aa<f8Arr1.Length ;aa++ )
       {
       iSourceIdx = aa;
       iFoundIdx = Array.BinarySearch
	 (
	  f8Arr1
	  ,0 ,f8Arr1.Length
	  ,(double)f8Arr1[iSourceIdx] );
       if ( (double)f8Arr1[iFoundIdx] != (double)f8Arr1[iSourceIdx] )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_dib321:  f8Arr1 ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       sLoc="Loc_dib310";
       iFoundIdx = Array.BinarySearch
	 (
	  aAsArray
	  ,0 ,aAsArray.Length
	  ,(double)( aAsArray.GetValue( iSourceIdx ) )
	  );
       if ( (double)aAsArray.GetValue(iFoundIdx) != (double)aAsArray.GetValue(iSourceIdx) )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_dib456:  aAsArray-f8Arr1 ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       }
     aAsArray = Array.CreateInstance( typeof( double ) ,f8Arr2.Length );
     for ( aa=0 ;aa<f8Arr2.Length ;aa++ )
       {
       aAsArray.SetValue( (double)f8Arr2[aa] ,aa );
       }
     for ( aa=0 ;aa<f8Arr2.Length ;aa++ )
       {
       iSourceIdx = aa;
       iFoundIdx = Array.BinarySearch
	 (
	  f8Arr2
	  ,0 ,f8Arr2.Length
	  ,(double)f8Arr2[iSourceIdx] );
       if ( (double)f8Arr2[iFoundIdx] != (double)f8Arr2[iSourceIdx] )
	 {
	 if ( ! Double.IsNaN( (double)f8Arr2[iSourceIdx] ) )
	   {
	   bToReturn = false;
	   Console.WriteLine( "Error_fad321:  f8Arr2 ,"
			      + "iSourceIdx=="
			      + iSourceIdx.ToString() +" ,iFoundIdx=="
			      + iFoundIdx.ToString() );
	   }
	 }
       sLoc="Loc_fad310";
       iFoundIdx = Array.BinarySearch
	 (
	  aAsArray
	  ,0 ,aAsArray.Length
	  ,(double)( aAsArray.GetValue( iSourceIdx ) )
	  );
       if ( (double)aAsArray.GetValue(iFoundIdx) != (double)aAsArray.GetValue(iSourceIdx) )
	 {
	 if ( ! Double.IsNaN( (double)aAsArray.GetValue(iSourceIdx) ) )
	   {
	   bToReturn = false;
	   Console.WriteLine( "Error_fad456:  aAsArray-f8Arr2 ,"
			      + "iSourceIdx=="
			      + iSourceIdx.ToString() +" ,iFoundIdx=="
			      + iFoundIdx.ToString() );
	   }
	 }
       }
     aAsArray = Array.CreateInstance( typeof( double ) ,f8Arr3.Length );
     for ( aa=0 ;aa<f8Arr3.Length ;aa++ )
       {
       aAsArray.SetValue( (double)f8Arr3[aa] ,aa );
       }
     for ( aa=0 ;aa<f8Arr3.Length ;aa++ )
       {
       iSourceIdx = aa;
       iFoundIdx = Array.BinarySearch
	 (
	  f8Arr3
	  ,0 ,f8Arr3.Length
	  ,(double)f8Arr3[iSourceIdx] );
       if ( (double)f8Arr3[iFoundIdx] != (double)f8Arr3[iSourceIdx] )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_wex321:  f8Arr3 ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       sLoc="Loc_wex310";
       iFoundIdx = Array.BinarySearch
	 (
	  aAsArray
	  ,0 ,aAsArray.Length
	  ,(double)( aAsArray.GetValue( iSourceIdx ) )
	  );
       if ( (double)aAsArray.GetValue(iFoundIdx) != (double)aAsArray.GetValue(iSourceIdx) )
	 {
	 bToReturn = false;
	 Console.WriteLine( "Error_wex456:  aAsArray-f8Arr3 ,"
			    + "iSourceIdx=="
			    + iSourceIdx.ToString() +" ,iFoundIdx=="
			    + iFoundIdx.ToString() );
	 }
       }
     }
   catch ( Exception exc839 )
     {
     bToReturn = false;
     Console.WriteLine( "Error_wib204:  Unexpected Exception near sLoc="+ sLoc );
     Console.WriteLine( exc839.ToString() );
     }
   return bToReturn;
   } 
 internal static readonly Char[]   M_cArr   = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
 internal static readonly Int16[] M_i16Arr = {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3758BinarySearch_oiio oCbTest = new Co3758BinarySearch_oiio();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev + "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFAbbrev + s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
