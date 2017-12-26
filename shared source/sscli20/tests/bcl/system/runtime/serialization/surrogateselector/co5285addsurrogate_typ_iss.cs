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
public class Co5285AddSurrogate_typ_Iss
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SurrogateSelector.AddSurrogate(SurrogateInfo surrogates[])";
	public static String s_strTFName        = "Co5285AddSurrogate_typ_Iss.cs";
	public static String s_strTFAbbrev      = "Co5285";
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
				SurrogateSelector surrSelect;
				SurrogateSelector surrSelect_next;
				TestSerializationClass testclass;
				TestSerializationSurrogate testss, testss1, testss2;
				StreamingContext context = new StreamingContext(StreamingContextStates.CrossMachine);
				ISurrogateSelector ss;
				SurrogateSelector[] arrSelectors;
				Int32 iHowDeep;
				strLoc = "Loc_100aa";
				surrSelect = new SurrogateSelector();
				iCountTestcases++;
				try
				{
					surrSelect.AddSurrogate(this.GetType(), context, null);
					iCountErrors++;
					printerr("Error_100bb! Expected exception not thrown");
				}
				catch (ArgumentException){}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Error_100dd! Incorrect Exception thrown: "+exc.ToString());
				}
				strLoc = "Loc_200aa";
				surrSelect = new SurrogateSelector();
				testss = new TestSerializationSurrogate();
				iCountTestcases++;
				try
				{
					surrSelect.AddSurrogate(null, context, testss);
					iCountErrors++;
					printerr("Error_200bb! Expected exception not thrown");
				}
				catch (ArgumentException){}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Error_200dd! Incorrect exception thrown: "+exc.ToString());
				}
				strLoc = "Loc_300aa";
				surrSelect = new SurrogateSelector();
				testss = new TestSerializationSurrogate();
				testclass = new TestSerializationClass();
				surrSelect.AddSurrogate(testclass.GetType(), context, testss);
				iCountTestcases++;
				if(!surrSelect.GetSurrogate(testclass.GetType(), context, out ss).Equals(testss))
				{
					iCountErrors++;
					printerr("Error_300bb! Incorrect ISerializationSurrogate returned");
				}
				strLoc = "Loc_350aa";
				testss = new TestSerializationSurrogate();
				testss1 = new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				surrSelect = new SurrogateSelector();
				surrSelect.AddSurrogate(typeof(Decimal), context, testss);
				surrSelect.AddSurrogate(typeof(Int64),context, testss1);
				surrSelect.AddSurrogate(typeof(Int32),context, testss2);
				iCountTestcases++;
				if(!surrSelect.GetSurrogate(typeof(Decimal), context, out ss).Equals(testss))
				{
					iCountErrors++;
					printerr("Error_350bb! Incorrect ISerializationSurrogate returned");
				}
				iCountTestcases++;
				if(surrSelect != ss)
				{
					iCountErrors++;
					printerr("Error_0453cd! Incorrect ISurrogateSelector returned");
				}
				iCountTestcases++;
				if(!surrSelect.GetSurrogate(typeof(Int64), context, out ss).Equals(testss1))
				{
					iCountErrors++;
					printerr("Error_350cc! Incorrect ISerializationSurrgate returned");
				}
				iCountTestcases++;
				if(!surrSelect.GetSurrogate(typeof(Int32), context, out ss).Equals(testss2))
				{
					iCountErrors++;
					printerr("Error_350dd! Incorrect IserializationSurrogate returned");
				}
				strLoc = "Loc_370aa";
				testss = new TestSerializationSurrogate();
				testss1 = new TestSerializationSurrogate();
				testss2 = new TestSerializationSurrogate();
				surrSelect = new SurrogateSelector();
				surrSelect.AddSurrogate(typeof(Decimal), context,  testss);
				surrSelect.AddSurrogate(typeof(Int64),context,  testss1);
				surrSelect.AddSurrogate(typeof(Int32),context,  testss2);
				iCountTestcases++;
				if(surrSelect.GetSurrogate(typeof(Int16), context, out ss) != null)
				{
					iCountErrors++;
					printerr("Error_370bb! Type not in surrogateselector was actually found???");
				}
				strLoc = "Loc_400aa";
				surrSelect = new SurrogateSelector();
				testss = new TestSerializationSurrogate();
				testclass = new TestSerializationClass();
				surrSelect.AddSurrogate(testclass.GetType(),context, testss);
				try
				{
					surrSelect.AddSurrogate(testclass.GetType(),context, new TestSerializationSurrogate());
					iCountErrors++;
					printerr("Error_400bb! Expected Exception not thrown");
				}catch (ArgumentException){
				}catch (Exception exc){
					iCountErrors++;
					printerr("Error_400cc! Incorrect Exception thrown : "+exc.ToString());
				}
				surrSelect = new SurrogateSelector();
				surrSelect_next = new SurrogateSelector();
				surrSelect.ChainSelector(surrSelect_next);
				testss = new TestSerializationSurrogate();
				context = new StreamingContext(StreamingContextStates.All);
				surrSelect_next.AddSurrogate(typeof(Decimal), context, testss);
				iCountTestcases++;
				if(!surrSelect.GetSurrogate(typeof(Decimal), context, out ss).Equals(testss))
				{
					iCountErrors++;
					printerr("Error_04582fd! Incorrect ISerializationSurrogate returned");
				}
				iCountTestcases++;
				if(surrSelect == ss)
				{
					iCountErrors++;
					printerr("Error_0478523vdf! Incorrect ISurrogateSelector returned");
				}
				iCountTestcases++;
				if(surrSelect_next != ss)
				{
					iCountErrors++;
					printerr("Error_934721cd! Incorrect ISurrogateSelector returned");
				}
				iHowDeep = 100;
				arrSelectors = new SurrogateSelector[iHowDeep];
				arrSelectors[0] = new SurrogateSelector();
				for(int i=1;i<iHowDeep;i++){
					arrSelectors[i] = new SurrogateSelector();
					arrSelectors[0].ChainSelector(arrSelectors[i]);
				}
				testss = new TestSerializationSurrogate();
				context = new StreamingContext(StreamingContextStates.All);
				arrSelectors[1].AddSurrogate(typeof(Decimal), context, testss);
				iCountTestcases++;
				if(!arrSelectors[0].GetSurrogate(typeof(Decimal), context, out ss).Equals(testss))
				{
					iCountErrors++;
					printerr("Error_743cd! Incorrect ISerializationSurrogate returned");
				}
				iCountTestcases++;
				if(arrSelectors[1] != ss)
				{
					iCountErrors++;
					printerr("Error_10573cd! Incorrect ISurrogateSelector returned");
				}
				arrSelectors[0].AddSurrogate(typeof(Decimal), context, testss);
				iCountTestcases++;
				if(!arrSelectors[0].GetSurrogate(typeof(Decimal), context, out ss).Equals(testss))
				{
					iCountErrors++;
					printerr("Error_0472ds! Incorrect ISerializationSurrogate returned");
				}
				iCountTestcases++;
				if(arrSelectors[0] != ss)
				{
					iCountErrors++;
					printerr("Error_0342dfg! Incorrect ISurrogateSelector returned");
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
		Co5285AddSurrogate_typ_Iss cbA = new Co5285AddSurrogate_typ_Iss();
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
