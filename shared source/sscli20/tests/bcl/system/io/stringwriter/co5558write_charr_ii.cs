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
using System.Text;
using System.IO;
using System.Collections;
using System.Globalization;
public class Co5558Write_chArr_ii
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StringWriter.Write(Char[], Int32, Int32)";
	public static String s_strTFName        = "Co5558Write_chArr_ii.cs";
	public static String s_strTFAbbrev      = "Co5558";
	public static String s_strTFPath        = Environment.CurrentDirectory;
    Random rand = new Random( (int) DateTime.Now.Ticks);
    int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue, Int16.MinValue};
    int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
    int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/2000 , Int32.MaxValue/5000, Int16.MaxValue };
    public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strValue = String.Empty;
        Char[] chArr = new Char[]{
			Char.MinValue
			,Char.MaxValue
			,'\t'
			,' '
			,'$'
			,'@'
			,'#'
			,'\0'
			,'\v'
			,'\''
			,'\u3190'
			,'\uC3A0'
			,'A'
			,'5'
			,'\uFE70' 
			,'-'
			,';'
			,'\u00E6'
		};
		try {
			StringWriter sw = new StringWriter();
			iCountTestcases++;
			try {
				iCountTestcases++;
				sw.Write(null, 0, 0);
				iCountErrors++;
				printerr( "Error_5787s! Expected exception not thrown, sw=="+sw.ToString());
			} catch (ArgumentNullException) {
		    }
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "ERror_91098! Unexpected exception thrown, exc=="+exc.ToString());
		}
        iCountTestcases++;
        for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
    		try {
    			StringWriter sw = new StringWriter();
    			try {
    				sw.Write(chArr, iArrInvalidValues[iLoop], 0);
    				iCountErrors++;
    				printerr( "Error_298vy! Expected exception not thrown, sw=="+sw.ToString());
    			} catch (ArgumentOutOfRangeException) {
    			} 
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_98y89! Unexpected exception thrown, exc=="+exc.ToString());
    		} 
        }
        iCountTestcases++;
        for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
    		try {
    			StringWriter sw = new StringWriter();
    			try {
    				sw.Write(chArr, 0, iArrInvalidValues[iLoop]);
    				iCountErrors++;
    				printerr( "Error_209ux! Expected exception not thrown, sw=="+sw.ToString());
    			} catch (ArgumentOutOfRangeException) {
    			}
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_298cy! Unexpected exception thrown, exc=="+exc.ToString());
    		}
        }
        iCountTestcases++;
        for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
            try {
    			StringWriter sw = new StringWriter();
    			try {
    				sw.Write(chArr, iArrLargeValues[iLoop], chArr.Length);
    				iCountErrors++;
    				printerr( "Error_20deh! Expected exception not thrown, sw=="+sw.ToString());
    			} catch (ArgumentException) {
    			} 
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_298gs! Unexpected exception thrown, exc=="+exc.ToString());
    		}
        }
        iCountTestcases++;
        for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
            try {
    			StringWriter sw = new StringWriter();
    			try {
    				sw.Write(chArr, 1, iArrLargeValues[iLoop]);
    				iCountErrors++;
    				printerr( "Error_20deh! Expected exception not thrown, sw=="+sw.ToString());
    			} catch (ArgumentException) {
    			} 
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_298gs! Unexpected exception thrown, exc=="+exc.ToString());
    		}
        }
		try {
			StringBuilder sb = new StringBuilder(40);
			StringWriter sw = new StringWriter(sb);
			StringReader sr;
			sw.Write(chArr, 0, chArr.Length);
			sr = new StringReader(sw.GetStringBuilder().ToString());
			Int32 tmp = 0;
			for(int i = 0 ; i < chArr.Length ; i++) {
				iCountTestcases++;
				if((tmp = sr.Read()) != (Int32)chArr[i]) {
					iCountErrors++;
					printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
		}
		try {
			StringWriter sw = new StringWriter();
			StringReader sr;
			sw.Write(chArr, 2, 5);
			sr = new StringReader(sw.ToString());
			Int32 tmp = 0;
			for(int i = 2 ; i < 7 ; i++) {
				iCountTestcases++;
				if((tmp = sr.Read()) != (Int32)chArr[i]) {
					iCountErrors++;
					printerr( "Error_2980x_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_938t7! Unexpected exception thrown, exc=="+exc.ToString());
		} 
        iCountTestcases++ ;
        for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
            try {
    			StringBuilder sb = new StringBuilder(Int32.MaxValue/2000);
    			StringWriter sw = new StringWriter(sb);
                chArr = new Char[ Int32.MaxValue/2000 ];
                for(int i = 0 ; i < chArr.Length ; i++ )
                    chArr[i] = Convert.ToChar( rand.Next(Char.MinValue , 10000 ) );
    			sw.Write(chArr, iArrValidValues[iLoop] -1, 1);
    			String strTemp =sw.GetStringBuilder().ToString();
                if( strTemp.Length != 1) {
                    iCountErrors++;
                    printerr( "Error_5450!!!! Expected==1, got=="+strTemp.Length);
                }
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_7875! Unexpected exception thrown, exc=="+exc.ToString());
    		}
        }
        iCountTestcases++ ;
        for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
            try {
    			StringBuilder sb = new StringBuilder(Int32.MaxValue/2000);
    			StringWriter sw = new StringWriter(sb);
                chArr = new Char[ Int32.MaxValue/2000 ];
                for(int i = 0 ; i < chArr.Length ; i++ )
                    chArr[i] = Convert.ToChar( rand.Next(Char.MinValue , 10000 ) );
    			sw.Write(chArr, 0, iArrValidValues[iLoop]);
    			String strTemp =sw.GetStringBuilder().ToString();
                if( strTemp.Length != iArrValidValues[iLoop]) {
                    iCountErrors++;
                    printerr( "Error_8450!!!! Expected==1, got=="+strTemp.Length);
                }
    		} catch (Exception exc) {
    			iCountErrors++;
    			printerr( "Error_4432! Unexpected exception thrown, exc=="+exc.ToString());
    		}
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
		Co5558Write_chArr_ii cbA = new Co5558Write_chArr_ii();
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
