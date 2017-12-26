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
using System.Resources;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
public class Co8636GetNextSelector
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SurrogateSelector.GetNextSelector()";
	public static String s_strTFName        = "Co8636GetNextSelector.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		SurrogateSelector selector1;
		SurrogateSelector selector2;
		SurrogateSelector[] selectorArr;
		Int32 numberOfSelectors;
		SurrogateSelector selectorReturned;
		try {
			strLoc = "Loc_98374sdg";
			iCountTestcases++;
			selector1 = new SurrogateSelector();
			selector2 = new SurrogateSelector();
			selector1.ChainSelector(selector2);
			selectorReturned = (SurrogateSelector)selector1.GetNextSelector();
			if(selectorReturned != selector2){
				iCountErrors++;
				Console.WriteLine("Err_34905wdsg! Wrong value returned");
			}
			strLoc = "Loc_98374sdg";
			iCountTestcases++;
			selector1 = new SurrogateSelector();
			selectorReturned = (SurrogateSelector)selector1.GetNextSelector();
			if(selectorReturned != null){
				iCountErrors++;
				Console.WriteLine("Err_93475sdg! Wrong value returned");
			}
			strLoc = "Loc_98374sdg";
			iCountTestcases++;
			selector1 = new SurrogateSelector();
			numberOfSelectors = 50;
			selectorArr = new SurrogateSelector[numberOfSelectors];
			for(int i=0; i<numberOfSelectors; i++){
				selectorArr[i] = new SurrogateSelector();
				selector1.ChainSelector(selectorArr[i]);
			}
			for(int i=numberOfSelectors-1; i>=0; i--){
				selectorReturned = (SurrogateSelector)selector1.GetNextSelector();
				if(selectorReturned != selectorArr[i]){
					iCountErrors++;
					Console.WriteLine("Err_8345sdg_" + i + "! Wrong value returned");
				}				
				selector1 = selectorReturned;
			}
		} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
			Console.WriteLine(exc_general.StackTrace);
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
		Co8636GetNextSelector cbA = new Co8636GetNextSelector();
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
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
class TestSerializationSurrogate : ISerializationSurrogate
{
	public void GetObjectData(Object obj, SerializationInfo si, StreamingContext context) {
	}
	public Object SetObjectData(Object obj, SerializationInfo info, StreamingContext context, ISurrogateSelector selector) {
		return obj;
	}
}
