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
public class Co9401Write_bool
{
    Random rand = new Random( (int) DateTime.Now.Ticks );
	public bool runTest()
	{
		int iCountErrors = 0;
		int iCountTestcases = 0;
        Char[] cArr = new Char[5] ; ;
        StringBuilder sb = new StringBuilder(40);
        StringWriter sw = new StringWriter(sb);
        StringReader sr;
        iCountTestcases++;
        bool[] bArr = new bool[]{  true,true,true,true,true,false,false,false,false,false};
		try {
			for(int i = 0 ; i < bArr.Length ; i++)
				sw.Write(bArr[i]);
			sr = new StringReader(sw.GetStringBuilder().ToString());
			for(int i = 0 ; i < bArr.Length ; i++) {
                sr.Read(cArr, 0, bArr[i].ToString().Length);
				if(new String(cArr, 0, bArr[i].ToString().Length) != bArr[i].ToString()) {
					iCountErrors++;
					printerr( "Error_298vc_"+i+"! Expected=="+bArr[i].ToString()+", got=="+new String(cArr));
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
		}
        iCountTestcases++;
        bArr = new bool[10000];
        for(int i = 0 ; i < bArr.Length ; i++)
            bArr[i] = Convert.ToBoolean(rand.Next(0,2));
		try {
            sb.Length = 0;
			for(int i = 0 ; i < bArr.Length ; i++)
				sw.Write(bArr[i]);
			sr = new StringReader(sw.GetStringBuilder().ToString());
			for(int i = 0 ; i < bArr.Length ; i++) {
                sr.Read(cArr, 0, bArr[i].ToString().Length);
				if(new String(cArr, 0, bArr[i].ToString().Length) != bArr[i].ToString()) {
					iCountErrors++;
					printerr( "Error_5754_"+i+"! Expected=="+bArr[i].ToString()+", got=="+new String(cArr));
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_0845! Unexpected exception thrown, exc=="+exc.ToString());
		}
        if ( iCountErrors == 0 ) {
			Console.WriteLine( "paSs. iCountTestcases=="+iCountTestcases.ToString());
			return true;
		}else {
			Console.WriteLine("FAiL! iCountErrors=="+iCountErrors.ToString() );
			return false;
		}
	}
	public void printerr ( String err )
	{
		Console.WriteLine ("POINTTOBREAK: "+ err);
	}
	public void printinfo ( String info )
	{
		Console.WriteLine ("INFO: "+ info);
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co9401Write_bool cbA = new Co9401Write_bool();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(" : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
