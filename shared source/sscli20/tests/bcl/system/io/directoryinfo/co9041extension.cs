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
using System.Text;
public class Co9041Extension
{
	public static String s_strDtTmVer       = "";
        public static String s_strClassMethod   = "DirectoryInfo.Extension";
	public static String s_strTFName        = "Co9041Extension.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		String strLoc = "Loc_000oo";
		String strValue = String.Empty;
		int iCountErrors = 0;
		int iCountTestcases = 0;
		try
		{
                        String fileName = "Testing";
                        DirectoryInfo dir ;  		
			strLoc = "Err_0001";
			iCountTestcases++;
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != "") {
				iCountErrors++;
				printerr( "Error_0002! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_0003";
			iCountTestcases++;
                        fileName =  "foo.bar";
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != ".bar" ) {
				iCountErrors++;
				printerr( "Error_0004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_0006";
			iCountTestcases++;
			dir = new DirectoryInfo(".");
                        if( dir.Extension != "" ) {
				iCountErrors++;
				printerr( "Error_0007! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_1003";
			iCountTestcases++;
                        fileName =  "foo.bar.fkl;fkds92-509450-4359.$#%()#%().%#(%)_#(%_).cs";
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != ".cs" ) {
				iCountErrors++;
				printerr( "Error_1004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_2003";
                        iCountTestcases++;
                        String extension = ".bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
                        fileName =  "AAAAAAAAAAAAAAAAAAAAAA" + extension ;
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != extension ) {
				iCountErrors++;
				printerr( "Error_2004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_3003";
                        iCountTestcases++;
                        fileName =  "foo.";
			dir = new DirectoryInfo(fileName);
                        String test = Exten( fileName );
                        if( dir.Extension != "" ) {
				iCountErrors++;
				printerr( "Error_3004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_4003";
                        iCountTestcases++;
                        extension = ".$#@$_)+_)!@@!!@##&_$)#_";
                        fileName =  "foo" + extension ;
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != extension ) {
				iCountErrors++;
				printerr( "Error_4004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_5003";
                        iCountTestcases++;
                        extension = "..............";
                        fileName =  "foo" + extension ;
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != "" ) {
				iCountErrors++;
				printerr( "Error_5004! Incorrect extension , dir=="+dir.Extension);
			}
			strLoc = "Err_6003";
                        iCountTestcases++;
                        extension = "..............";
                        fileName =  "foo . z" + extension ;
			dir = new DirectoryInfo(fileName);
                        if( dir.Extension != ". z" ) {
				iCountErrors++;
				printerr( "Error_6004! Incorrect extension , dir=="+dir.Extension);
			}
			if(File.Exists(fileName))
				File.Delete(fileName);
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
			Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
			return false;
		}
	}    
        private string RemoveIfDotExists( String str ){
                while( str.IndexOf(".") > -1 )
                        str = str.Remove( str.IndexOf(".") , 1 );
                return str ;        
        }
		public String Exten( String str) 
		{
			   int length = str.Length;
				for (int i = length; --i >= 0;) {
					char ch = str[i];
					Console.WriteLine( ch );
                                        if (ch == '.')
						return str.Substring(i, length - i);
					if (ch == Path.DirectorySeparatorChar || ch == Path.AltDirectorySeparatorChar || ch == Path.VolumeSeparatorChar)
						break;
				}
				return String.Empty;
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
		Co9041Extension cbA = new Co9041Extension();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
                }
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
