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
public class Co5287ChainSelector
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SurrogateSelector.ChainSelector(SurrogateInfo surrogates[])";
	public static String s_strTFName        = "Co5287ChainSelector.cs";
	public static String s_strTFAbbrev      = "Co5287";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		try {
			do
			{
				SurrogateSelector surrSelect1, surrSelect2, surrSelect3, surrSelect4, surrSelect5;
				TestSerializationSurrogate testss1, testss2, testss3, testss4, testss5;
				StreamingContext context = new StreamingContext(StreamingContextStates.CrossMachine);
				ISurrogateSelector ss;
				SurrogateSelector[] arrSelectors;
				Int32 iHowDeep;
				strLoc = "Loc_100aa";
				surrSelect1 = new SurrogateSelector();
				iCountTestcases++;
				try
				{
					surrSelect1.ChainSelector(null);
					iCountErrors++;
					printerr("Error_100bb!! Expected ArgumentException not thrown");
				}
				catch (ArgumentException){}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Error_100dd!! Incorrect Exception thrown : "+exc.ToString());
				}
				strLoc = "Loc_300aa";
				iCountTestcases++;
				surrSelect1 = new SurrogateSelector();
				try
				{
					surrSelect1.ChainSelector(surrSelect1);
				}catch (SerializationException){
				}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_432fs!! Incorrect Exception thrown : "+ex.ToString());
				}
				if(surrSelect1.GetSurrogate(typeof(Int64), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_2139d!!!!");
				}
				strLoc = "Loc_350aa";
				testss1= new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				testss3 = new TestSerializationSurrogate();
				surrSelect1 = new SurrogateSelector();
				surrSelect1.AddSurrogate(typeof(Int64), context,  testss2);
				surrSelect1.AddSurrogate(typeof(Int32), context,  testss3);
				iCountTestcases++;
				if(!surrSelect1.GetSurrogate(typeof(Int64), context, out ss).Equals(testss2))
				{
					iCountErrors++;
					printerr("Error_350cc! Incorrect ISerializationSurrgate returned");
				}
				iCountTestcases++;
				if(!surrSelect1.GetSurrogate(typeof(Int32), context, out ss).Equals(testss3))
				{
					iCountErrors++;
					printerr("Error_350dd! Incorrect IserializationSurrogate returned");
				}
				strLoc = "Loc_370aa";
				testss1= new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				testss3 = new TestSerializationSurrogate();
				surrSelect1 = new SurrogateSelector();
				surrSelect1.AddSurrogate(typeof(Int64), context,  testss2);
				surrSelect1.AddSurrogate(typeof(Int32), context,  testss3);
				strLoc = "Loc_400aa";
				surrSelect1 = new SurrogateSelector();
				surrSelect2 = new SurrogateSelector();
				surrSelect3 = new SurrogateSelector();
				surrSelect4 = new SurrogateSelector();
				surrSelect5 = new SurrogateSelector();
				testss1 = new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				testss3 = new TestSerializationSurrogate();
				testss4 = new TestSerializationSurrogate();
				testss5 = new TestSerializationSurrogate();
				surrSelect3.ChainSelector(surrSelect5);
				surrSelect3.ChainSelector(surrSelect4);
				surrSelect2.ChainSelector(surrSelect3);
				surrSelect1.ChainSelector(surrSelect2);
				surrSelect5.AddSurrogate(typeof(Single), context,  testss5);
				iCountTestcases++;
				if(!(surrSelect1.GetSurrogate(typeof(Single), context, out ss).Equals(testss5)))
				{
					iCountErrors++;
					printerr("Error_400bb!! Did not find the surrogate through the chain");
				}
				iCountTestcases++;
				if(!(surrSelect3.GetSurrogate(typeof(Single), context, out ss).Equals(testss5)))
				{
					iCountErrors++;
					printerr("Error_400cc!! Did not find the surrogate through the chain");
				}
				iCountTestcases++;
				if(!(surrSelect4.GetSurrogate(typeof(Single), context, out ss).Equals(testss5)))
				{
					iCountErrors++;
					printerr("Error_400dd!! Did not find the surrogate through the chain");
				}
				strLoc = "Loc_401aa";
				surrSelect1.AddSurrogate(typeof(Guid), context,  testss1);
				iCountTestcases++;
				if(!(surrSelect1.GetSurrogate(typeof(Guid), context, out ss).Equals(testss1)))
				{
					iCountErrors++;
					printerr("Error_401bb!! Did not find the surrogate through the chain");
				}
				iCountTestcases++;
				if(surrSelect2.GetSurrogate(typeof(Guid), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_401cc!! Found surrogate that is above in the chain");
				}
				strLoc = "Loc_402aa";
				surrSelect2.AddSurrogate(typeof(Decimal), context,  testss2);
				iCountTestcases++;
				if(!(surrSelect1.GetSurrogate(typeof(Decimal), context, out ss).Equals(testss2)))
				{
					iCountErrors++;
					printerr("Error_402bb!! Didn't find surrogate of child");
				}
				strLoc = "Loc_403aa";
				surrSelect3.AddSurrogate(typeof(Int32), context,  testss3);
				iCountTestcases++;
				if(!(surrSelect1.GetSurrogate(typeof(Int32), context, out ss).Equals(testss3)))
				{
					iCountErrors++;
					printerr("Error_403bb!! Didn't find surrogate of child");
				}
				strLoc = "Loc_404aa";
				surrSelect4.AddSurrogate(typeof(Exception), context,  testss4);
				iCountTestcases++;
				if(!(surrSelect1.GetSurrogate(typeof(Exception), context, out ss).Equals(testss4)))
				{
					iCountErrors++;
					printerr("Error_404bb!! Didn't find surrogate of child");
				}
				strLoc = "Loc_405aa";
				if(surrSelect1.GetSurrogate(typeof(Int16), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_405bb!! Found surrogate not in chain");
				}
				iHowDeep = 10;
				arrSelectors = new SurrogateSelector[iHowDeep];
				arrSelectors[0] = new SurrogateSelector();
				for(int i=1;i<iHowDeep;i++){
					arrSelectors[i] = new SurrogateSelector();
					arrSelectors[0].ChainSelector(arrSelectors[i]);
				}
				iCountTestcases++;
				if(arrSelectors[0].GetNextSelector() != arrSelectors[iHowDeep-1])
				{
					iCountErrors++;
					printerr("Error_03842vdsd! Incorrect ISerializationSurrgate returned");
				}
				for(int i=iHowDeep-1;i>1;i--){
					iCountTestcases++;
					if(arrSelectors[i].GetNextSelector() != arrSelectors[i-1]){
						iCountErrors++;
						printerr("Err_74239cd_" + i + "! Incorrect ISerializationSurrgate returned");
					}
				}
				iCountTestcases++;
				if(arrSelectors[1].GetNextSelector() != null)
				{
					iCountErrors++;
					printerr("Error_0483vf! Incorrect ISerializationSurrgate returned");
				}
				try {
					iCountTestcases++;
					arrSelectors[2].ChainSelector(arrSelectors[8]);
					context = new StreamingContext(StreamingContextStates.All);
					testss1 = (TestSerializationSurrogate)arrSelectors[0].GetSurrogate(typeof(Int64), context, out ss);
					iCountErrors++;
					Console.WriteLine("Err_74239cd!! Expected Exception not thrown");
				}catch(ArgumentException){
				}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_652df!! Incorrect Exception thrown : "+ex.ToString());
				}
			} while (false);
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
		Co5287ChainSelector cbA = new Co5287ChainSelector();
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
class TestSerializationClass
{
	public Int64 i64;
	public Int32 i32;
	public Int16 i16;
	public String strpub;
	private String strpriv;
	protected String strprot;
	public Decimal dec2;
	public Double dbl2;
	public String GetMyPublicString()
	{
		return strpub;
	}
	public String GetMyPrivateString()
	{
		return strpriv;
	}
	public void SetMyPrivateString(String s)
	{
		strpriv = s;
	}
	public String GetMyProtectedString()
	{
		return strprot;
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
