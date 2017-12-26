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
using System;
using System.IO;
public class Co5387CompareTo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal(usigned int)";
 public static String s_strTFName        = "Co5387CompareTo.cs";
 public static String s_strTFAbbrev      = "Co5387";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public static Random rand               = new Random( ( ( DateTime.Now.Second * 1000 ) + DateTime.Now.Millisecond ) );
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     Decimal dec1;
     Decimal[] dcmlFirstValues = new Decimal[]{Decimal.MaxValue, Decimal.MinusOne,
					       Decimal.Parse("1.4214206543486529434634231456"),
					       new Decimal(0.0001), Decimal.Zero, new Decimal(Int64.MaxValue),
					       Decimal.MaxValue, new Decimal(1234.5678), new Decimal(-1234.5677),
					       new Decimal(1234.5678)};
     Decimal[] dcmlSecondValues = new Decimal[]{Decimal.One, Decimal.MinValue,
						Decimal.Parse("-7.9228162514264337593543950335"),
						new Decimal(-0.0001), new Decimal(-0.0001), new Decimal(Int64.MinValue),
						Decimal.MinValue, new Decimal(-1234.5678), new Decimal(-1234.5678),
						new Decimal(1234.5677)};
     Decimal[] dcmlExceptions = new Decimal[]{};
     int ina;
     for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
       {
       ina = dcmlFirstValues[aa].CompareTo(dcmlSecondValues[aa]);
       ++iCountTestcases;
       if ( ina != 1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa.ToString() + "  ina ==" + ina.ToString()  );
	 }
       }
     for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
       {
       ina = dcmlSecondValues[aa].CompareTo(dcmlFirstValues[aa]);
       ++iCountTestcases;
       if ( ina != -1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_346mk_" + aa.ToString() + "  ina ==" + ina.ToString()  );
	 }
       }
     for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
       {
       ina = dcmlFirstValues[aa].CompareTo(dcmlFirstValues[aa]);
       ++iCountTestcases;
       if ( ina != 0)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_934qm_" + aa.ToString() + "  ina ==" + ina.ToString()  );
	 }
       }
     for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
       {
       ina = dcmlSecondValues[aa].CompareTo(dcmlSecondValues[aa]);
       ++iCountTestcases;
       if ( ina != 0)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_134ak_" + aa.ToString() + "  ina ==" + ina.ToString()  );
	 }
       }
     for (int aa = 0; aa < dcmlExceptions.Length; aa++)
       {
       try
	 {
	 ++iCountTestcases;
	 ina = dcmlExceptions[aa].CompareTo(dcmlExceptions[aa]);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa.ToString() + "!  Exception not thrown"  );
	 }
       catch (OverflowException ex)
	 {
	 Console.WriteLine("Caught expected overflowException: "+ex.Message);
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_259pl_" + aa.ToString() + "!  , Wrong Exception thrown == " + ex.ToString()  );
	 }
       }
     strLoc = "Loc_298hc";
     dec1 = 5;
     iCountTestcases++;
     if(dec1.CompareTo(null) <= 0)
       {
       iCountErrors++;
       printerr("Error_297yv! Got negative or 0 value when passing in null");
       }
     strLoc = "Loc_983hv";
     dec1 = 2;
     iCountTestcases++;
     try
       {
       dec1.CompareTo( Convert.ToDouble("100.0"));
       iCountErrors++;
       printerr("Error_2938x! ArgumentException expected, got value=="+dec1.CompareTo(new Decimal(3)).ToString());
       }
     catch (ArgumentException aexc)
       {
       Console.WriteLine("Info_347xz! Caught Expected ArgumentException , exc=="+aexc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298yx! ArgumentException expected, exc=="+exc.ToString());
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5387CompareTo cbA = new Co5387CompareTo();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
