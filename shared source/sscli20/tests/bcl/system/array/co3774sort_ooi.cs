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
using System.Collections;
using System.Text;
public class AIcmp : IComparer
{
 Int32 _iValue;
 public AIcmp()
   {
   _iValue = rnd.Next(100);
   }
 override public String ToString()
   {
   StringBuilder sb = new StringBuilder();
   sb.Append(_iValue);
   return sb.ToString();
   }
 public int Compare(Object object1, Object object2)
   {
   if (object1 == null) {
   return 1;
   }
   if ((object1 is AIcmp) && (object2 is AIcmp)) {
   Int32 i = Convert.ToInt32(object1.ToString());
   Int32 j = Convert.ToInt32(object2.ToString());
   if (j < i) return 1;
   if (j > i) return -1;
   return 0;
   }
   throw new ArgumentException ("Object must be of type AIcmp");
   }
 static Random rnd = new Random();
}
public class Co3774Sort_ooi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.Sort(Object[], Object[], IComparer)";
 public static String s_strTFName        = "Co3774Sort_ooi";
 public static String s_strTFAbbrev      = "Co3774";
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   AIcmp oKey;
   Guid oValue;
   Int32 iPrevious;
   AIcmp[] aArr;
   Guid[] gArr;
   Hashtable hshtbl1;
   AIcmp acmp = new AIcmp();
   try
     {
     Console.WriteLine();
     do
       {
       aArr = new AIcmp[10];
       gArr = new Guid[10];
       hshtbl1 = new Hashtable(16);
       for(int i=0; i < 10; i++)
	 {
	 aArr[i] = new AIcmp();
	 gArr[i] = Guid.NewGuid();
	 hshtbl1.Add(gArr[i], aArr[i]);
	 }
       Array.Sort(aArr, gArr, acmp);
       iPrevious = 0;	
       for(int i=0; i < aArr.Length; i++)
	 {
	 oKey = aArr[i];
	 oValue = gArr[i];
	 ++iCountTestcases;
	 if ( Convert.ToInt32(oKey.ToString()) < iPrevious )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_634df_" + i + ", oKey.ToString()==" + oKey.ToString());
	   }
	 iPrevious=Convert.ToInt32(oKey.ToString());
	 ++iCountTestcases;
	 if ( oKey.Equals(hshtbl1[oValue]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_364df_" + i + ", oKey.ToString()==" + oKey.ToString() + ", hshtbl1[oValue]==" + hshtbl1[oValue]);
	   }
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(gArr, i32Arr, null);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_984fd!" );
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_795gd!  , Wrong Exception thrown == " + ex.ToString());
	 Console.WriteLine(ex.StackTrace);
	 }
       strLoc = "loc_845fdg";
       aArr = new AIcmp[10];
       gArr = new Guid[9];
       for(int i=0; i < 9; i++)
	 {
	 aArr[i] = new AIcmp();
	 gArr[i] = Guid.NewGuid();
	 }
       aArr[9]=new AIcmp();
       strLoc = "loc_086fd";
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(aArr, gArr, acmp );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!" );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_972qr!  , Wrong Exception thrown == " + ex.ToString());
	 }
       aArr = new AIcmp[10];
       for(int i=0; i < 10; i++)
	 {
	 aArr[i] = new AIcmp();
	 }
       Array.Sort(aArr, null, acmp);
       iPrevious = 0;	
       ++iCountTestcases;
       for(int i=0; i < aArr.Length; i++)
	 {
	 oKey = aArr[i];
	 if ( Convert.ToInt32(oKey.ToString()) < iPrevious )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + ", Err_573df_" + i + ", oKey.ToString()==" + oKey.ToString());
	   }
	 iPrevious=Convert.ToInt32(oKey.ToString());
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(null, gArr, acmp);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!" );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_972qr!  , Wrong Exception thrown == " + ex.ToString());
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
 static Char[]			cArr				 = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
 static Char[]			cSortedArr	 = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
 static Int16[]		i16Arr				= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int16[]		iKeySorted16Arr	= {753, -563, 60, -52, 0, 565, 6, 317, 238, 19};
 static Int32[]		i32Arr				= {2, 3, 1};
 static Int32[]		i32SortedArr	= {1, 2, 3};
 static Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 static Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
					     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3774Sort_ooi oCbTest = new Co3774Sort_ooi();
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
