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
using System.Collections;
using System.Globalization;
public class Co5554Write_bArr_ii
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BinaryWriter.Write(Byte[], Int32, Int32)";
	public static String s_strTFName        = "Co5554Write_bArr_ii.cs";
	public static String s_strTFAbbrev      = "Co5554";
	public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
        Random rand = new Random( (int) DateTime.Now.Ticks );
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue, Int16.MinValue};
        int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
        int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/2000 , Int32.MaxValue/10000, Int16.MaxValue };
		try
		{		
			BinaryWriter dw2 = null;
			BinaryReader dr2 = null;
			MemoryStream mstr = null;
			Byte[] bArr = new Byte[0];
			int ii = 0;
			Byte[] bReadArr = new Byte[0];
			Int32 ReturnValue;
			bArr = new Byte[1000];
			bArr[0] = Byte.MinValue;
			bArr[1] = Byte.MaxValue;
			for(ii = 2 ; ii < 1000 ; ii++)
				bArr[ii] = Convert.ToByte(rand.Next(Byte.MinValue, Byte.MaxValue));
			strLoc = "Loc_98yss";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			iCountTestcases++;
			try {
				Console.WriteLine(bArr.Length);
				dw2.Write(bArr, 0, bArr.Length);
				dw2.Flush();
				mstr.Position = 0;
				strLoc = "Loc_287y5";
				dr2 = new BinaryReader(mstr);
				bReadArr = new Byte[bArr.Length];
				ReturnValue = dr2.Read(bReadArr, 0, bArr.Length);
				iCountTestcases++;
				if(ReturnValue != bArr.Length) {
					iCountErrors++;
					printerr( "Error_589yv! Return value incorrect, ReturnValue=="+ReturnValue);
				}
				for(ii = 0 ; ii < bArr.Length ; ii++) { 
					if(bReadArr[ii] != bArr[ii]) {
						iCountErrors++;
						printerr( "Error_27t55! Expected=="+bArr[ii]+" ("+(Int32)bArr[ii]+") , got=="+bReadArr[ii]+" ("+(Int32)bReadArr[ii]);
					}
				}
   			} 
			catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_38f85! Unexpected exception, exc=="+exc.ToString());
			}
			mstr.Close();
			strLoc = "Loc_489vy";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			iCountTestcases++;
			try {
				dw2.Write((Byte[])null, 0, 0);
				iCountErrors++;
				printerr( "Error_398ty! Expected exception not thrown");
			} catch (ArgumentNullException) {
			} catch (Exception exc) {
				iCountErrors++;
				printerr( "Error_298t7! Unexpected exception, exc=="+exc.ToString());
			}
			mstr.Close();
			strLoc = "Loc_8949s";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
    			try {
    				dw2.Write(bArr, iArrInvalidValues[iLoop], 0);
    				iCountErrors++;
    				printerr( "Error_1288h! Expected exception not thrown");
    			} catch (ArgumentOutOfRangeException) {
    			} catch (Exception exc) {
    				iCountErrors++;
    				printerr( "Error_398yc! Unexpected exception, exc=="+exc.ToString());
    			}
            }
			mstr.Close();
			strLoc = "Loc_09u2s";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
    			try {
    				dw2.Write(bArr, 0, iArrInvalidValues[iLoop]);
    				iCountErrors++;
    				printerr( "Error_1870v! Expected exception not thrown");
    			} catch (ArgumentOutOfRangeException) {
    			} catch (Exception exc) {
    				iCountErrors++;
    				printerr( "Error_109xu! Incorrect exception thrown, exc=="+exc.ToString());
    			}
            }
            dw2.Close();
			strLoc = "Loc_489hv";
			mstr = new MemoryStream();
			dw2 = new BinaryWriter(mstr);
			iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
    			try {
    				dw2.Write(bArr, iArrLargeValues[iLoop], 0);
    				iCountErrors++;
    				printerr( "Error_2990f! Expected exception not thrown");
    			} catch (ArgumentException aexc) {
    				printinfo( "Info_23983! Caught expected exception, aexc=="+aexc.Message);
    			} catch (Exception exc) {
    				iCountErrors++;
    				printerr( "Error_2099j! Incorrect exception thrown, exc=="+exc.ToString());
    			}	   
            }
           dw2.Close();
           strLoc = "Loc_4343";
           mstr = new MemoryStream();
           dw2 = new BinaryWriter(mstr);
           iCountTestcases++;
           for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
               try {
                   dw2.Write(bArr, 0, iArrLargeValues[iLoop]);
                   iCountErrors++;
                   printerr( "Error_4354! Expected exception not thrown");
               } catch (ArgumentException aexc) {
                   printinfo( "Info_5099! Caught expected exception, aexc=="+aexc.Message);
               } catch (Exception exc) {
                   iCountErrors++;
                   printerr( "Error_5484! Incorrect exception thrown, exc=="+exc.ToString());
               }	   
           }
          dw2.Close();
          mstr = new MemoryStream();
          bArr = new Byte[Int32.MaxValue/2000 + 1];
          for(int iLoop = 0 ; iLoop < bArr.Length ; iLoop++ )
              bArr[iLoop] = Convert.ToByte( rand.Next( Byte.MinValue , Byte.MaxValue ));
          dw2 = new BinaryWriter(mstr);
          iCountTestcases++;
          for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
              try {
                  dw2.Write(bArr, iArrValidValues[iLoop], 1);
              } catch (Exception exc) {
                  iCountErrors++;
                  printerr( "Error_5435!!!! Incorrect exception thrown, exc=="+exc.ToString());
              }	   
          }
         dw2.Close();
         mstr = new MemoryStream();
         dw2 = new BinaryWriter(mstr);
         iCountTestcases++;
         for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
             try {
                 dw2.Write(bArr, 0, iArrValidValues[iLoop]);
             } catch (Exception exc) {
                 iCountErrors++;
                 printerr( "Error_6685!!!! Incorrect exception thrown, exc=="+exc.ToString());
             }	   
         }
        dw2.Close();
		} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
			return true;
		}
		else
		{
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	public void printerr ( String err )
	{
		Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
	}
	public void printinfo ( String info )
	{
		Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co5554Write_bArr_ii cbA = new Co5554Write_bArr_ii();
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
