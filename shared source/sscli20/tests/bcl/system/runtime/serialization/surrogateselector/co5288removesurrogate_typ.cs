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
public class Co5288RemoveSurrogate_typ
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SurrogateSelector.AddSurrogate(SurrogateInfo surrogates[])";
	public static String s_strTFName        = "Co5288RemoveSurrogate_typ.cs";
	public static String s_strTFAbbrev      = "Co5288";
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
				strLoc = "Loc_100aa";
				surrSelect1 = new SurrogateSelector();
				iCountTestcases++;
				try
				{
					surrSelect1.RemoveSurrogate(null, context);
					iCountErrors++;
					printerr("Error_100bb!! Expected ArgumentException not thrown");
				}
				catch (ArgumentException){}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Error_100dd!! Incorrect Exception thrown : "+exc.ToString());
				}
				strLoc = "Loc_200aa";
				surrSelect1 = new SurrogateSelector();
				iCountTestcases++;
				surrSelect1.RemoveSurrogate(typeof(String), context);
				strLoc = "Loc_350aa";
				testss1= new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				testss3 = new TestSerializationSurrogate();
				surrSelect1 = new SurrogateSelector();
				surrSelect1.AddSurrogate(typeof(Int64), context, testss2);
				surrSelect1.AddSurrogate(typeof(Int32), context, testss3);
				iCountTestcases++;
				if(!surrSelect1.GetSurrogate(typeof(Int64), context, out ss).Equals(testss2))
				{
					iCountErrors++;
					printerr("Error_350cc! Incorrect ISerializationSurrgate returned");
				}
				surrSelect1.RemoveSurrogate(typeof(Int64), context);
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Int64), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_350cd! ISerializationSurrgate not removed");
				}
				iCountTestcases++;
				if(!surrSelect1.GetSurrogate(typeof(Int32), context, out ss).Equals(testss3))
				{
					iCountErrors++;
					printerr("Error_350dd! Incorrect IserializationSurrogate returned");
				}
				surrSelect1.RemoveSurrogate(typeof(Int32), context);
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Int32), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_350de! SerializationSurrogate not removed properly");
				}
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
				surrSelect1.AddSurrogate(typeof(Guid), context, testss1);
				surrSelect2.AddSurrogate(typeof(Decimal), context, testss2);
				surrSelect3.AddSurrogate(typeof(Int32), context, testss3);
				surrSelect4.AddSurrogate(typeof(Exception), context, testss4);
				surrSelect5.AddSurrogate(typeof(Single), context, testss5);
				surrSelect3.RemoveSurrogate(typeof(Int64), context);
				if(surrSelect1.GetSurrogate(typeof(Int32), context, out ss) == null)
				{
					iCountErrors++;
					printerr("Error_400bb!! Did not find Surrogate in chain");
				}
				strLoc = "Loc_401aa";
				surrSelect1.RemoveSurrogate(typeof(Single), context);
				surrSelect1.RemoveSurrogate(typeof(Decimal), context);
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Single), context, out ss) == null)
				{
					iCountErrors++;
					printerr("Error_401bb! Incorrectly removed surrogates further down the chain");
				}
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Decimal), context, out ss) == null)
				{
					iCountErrors++;
					printerr("Error_401cc! Incorrectly removed suggogate further down the chain");
				}
				strLoc = "Loc_403aa";
				surrSelect3.RemoveSurrogate(typeof(Int32), context);
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Int32), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_403bb! Found a removed surrogate");
				}
				strLoc = "Loc_405aa";
				if(surrSelect1.GetSurrogate(typeof(Single), context, out ss) == null)
				{
					iCountErrors++;
					printerr("Error_405bb!! Surrogate in chain not found");
				}
				surrSelect5.RemoveSurrogate(typeof(Single), context);
				iCountTestcases++;
				if(surrSelect1.GetSurrogate(typeof(Single), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_405cc!! Removed surrogate found???");
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
			Console.WriteLine(exc_general.StackTrace);
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs.   "+s_strTFPath +"\\"+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
			return true;
		}
		else
		{
			Console.WriteLine("FAiL!   "+s_strTFPath+"\\"+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
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
		Co5288RemoveSurrogate_typ cbA = new Co5288RemoveSurrogate_typ();
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
