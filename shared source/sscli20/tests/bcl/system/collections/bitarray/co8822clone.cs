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
public class Co8822Clone
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "BitArray.Cloen";
	public static String s_strTFName        = "Co8822Clone.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		BitArray bitArr1;
		BitArray bitArr2;
		Boolean[] bolArr1;
		Int32 iNumOfElements;
		Random rnd1;
		try {
			do
			{
				iNumOfElements = 10;
				rnd1 = new Random();
				strLoc = "Loc_742dsf!";
				iCountTestcases++;
				bolArr1 = new Boolean[iNumOfElements];
				for(int i=0; i<iNumOfElements; i++){
					if(rnd1.Next(10)>5)
						bolArr1[i] = true;
					else
						bolArr1[i] = false;
				}
				bitArr1 = new BitArray(bolArr1);
				bitArr2 = (BitArray)bitArr1.Clone();
				for(int i=0; i<iNumOfElements; i++){
					if(bitArr1[i] != bitArr2[i]){
						iCountErrors++;
						Console.WriteLine("Err_36tdfg_" + i + "! Wrong value returned, ");
					}
				}
				iCountTestcases++;
				for(int i=0; i<iNumOfElements; i++)
					bitArr1[i] = !bitArr1[i];
				for(int i=0; i<iNumOfElements; i++){
					if(bitArr1[i] != !bitArr2[i]){
						iCountErrors++;
						Console.WriteLine("Err_394t7sg_" + i + "! Wrong value returned, ");
					}
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
			return true;
		}
		else
		{
			Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co8822Clone cbA = new Co8822Clone();
		try {
			bResult = cbA.runTest();
			} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
		}
		if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
	}
}
