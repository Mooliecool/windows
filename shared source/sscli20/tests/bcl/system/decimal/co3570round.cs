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
using System.Globalization;
using System;
public class Co3570Round
{
 internal static String strName = "Decimal.Round";
 internal static String strTest = "Co3570Round.cs";
 internal static String strPath = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal dcml2;
   Decimal dcmlExp;
   String str;
   String strExp;
   try
     {
     do
       {
       ++iCountTestcases;
       for(int aa = 1; aa < 28; aa++)
	 {
	 str = "0.";
	 for(int bb = 0; bb < aa; bb++)
	   str = str + "2";
	 str = str + "7";
	 dcml1 = Decimal.Parse(str, CultureInfo.InvariantCulture);
	 strExp = "0.";
	 for(int bb = 0; bb < (aa-1); bb++)
	   strExp = strExp + "2";
	 strExp = strExp + "3";
	 dcmlExp = Decimal.Parse(strExp, CultureInfo.InvariantCulture);
	 dcml2 = Decimal.Round(dcml1, aa);
	 if (dcml2 != dcmlExp)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_542er_" + aa + " dcml2 ==" + dcml2  );
	   }
	 }
       dcml1 = new Decimal(1231231321312312313213112.1235);
       dcml2 = Decimal.Round(dcml1, 3);
       dcmlExp = new Decimal(1231231321312312313213112.124);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(-1231231321312312313213112.1235);
       dcml2 = Decimal.Round(dcml1, 3);
       dcmlExp = new Decimal(-1231231321312312313213112.124);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_347wp! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(12);
       dcml2 = Decimal.Round(dcml1, 24);
       dcmlExp = new Decimal(12);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_367we! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(12.9999);
       dcml2 = Decimal.Round(dcml1, 4);
       dcmlExp = new Decimal(12.9999);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_359qp! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(12.9999);
       dcml2 = Decimal.Round(dcml1, 3);
       dcmlExp = new Decimal(13);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_459mk! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(0.3589);
       dcml2 = Decimal.Round(dcml1, 1);
       dcmlExp = new Decimal(0.4);
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_497re! dcml2 ==" + dcml2  );
	 }
       dcml1 = new Decimal(0.3589);
       dcml2 = Decimal.Round(dcml1, 0);
       dcmlExp = Decimal.Zero;
       ++iCountTestcases;
       if ( dcml2 != dcmlExp )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_349ad! dcml2 ==" + dcml2  );
	 }
       Random random = new Random();
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   dcml1 = Decimal.One;
	   ++iCountTestcases;
	   dcml2 = Decimal.Round(dcml1, random.Next(Int32.MinValue, 0));
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_346wo! Exception not thrown"  );
	   }
	 catch (ArgumentException){}
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_620qo! Wrong exception thrown == " + ex.ToString() );
	   }
	 }
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   dcml1 = Decimal.One;
	   ++iCountTestcases;
	   dcml2 = Decimal.Round(dcml1, random.Next(29, Int32.MaxValue));
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_349mk! Exception not thrown"  );
	   }
	 catch (ArgumentException){}
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_294qp! Wrong exception thrown == " + ex.ToString() );
	   }
	 }
       try
	 {
	 dcml1 = Decimal.One;
	 ++iCountTestcases;
	 dcml2 = Decimal.Round(dcml1, Int32.MaxValue);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_349mk! Exception not thrown"  );
	 }
       catch (ArgumentException){}
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_294qp! Wrong exception thrown == " + ex.ToString() );
	 }
       try
	 {
	 dcml1 = Decimal.One;
	 ++iCountTestcases;
	 dcml2 = Decimal.Round(dcml1, Int32.MinValue);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_349mk! Exception not thrown"  );
	 }
       catch (ArgumentException){}
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_294qp! Wrong exception thrown == " + ex.ToString() );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strPath + strTest + "  iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL! " + strPath + strTest + "   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3570Round oCbTest = new Co3570Round();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
