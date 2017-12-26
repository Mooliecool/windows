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
public class Co5244RegisterObject_Obj_lng
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "ObjectManager.RegisterObject(obj, int64)";
	public static String s_strTFName        = "Co5244RegisterObject_Obj_lng.cs";
	public static String s_strTFAbbrev      = "Co5244";
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public Boolean runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		try {
			do
			{
				ObjectManager objMan;
				String str1, str2;
				ISurrogateSelector isur = null;
				StreamingContext sc1 = new StreamingContext(StreamingContextStates.All);
				strLoc = "Loc_100aa";
				objMan = new ObjectManager(isur, sc1);
				iCountTestcases++;
				try
				{
					objMan.RegisterObject(null, 0);
					iCountErrors++;
					printerr("Error_100bb! Expected exception not thrown");
				}
				catch (ArgumentNullException) {
					}catch (Exception exc){
					iCountErrors++;
					printerr("Error_100cc! Incorrect Exception thrown: exc=="+exc.ToString());
				}
				strLoc = "Loc_200aa";
				str1 = "Test";
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject(str1, 1);
				iCountTestcases++;
				if(!((String)objMan.GetObject(1)).Equals(str1))
				{
					iCountErrors++;
					printerr("Error_200bb! Incorrect object returned");
				}
				strLoc = "Loc_300aa";
				str1 = "Test";
				objMan = new ObjectManager(isur, sc1);
				iCountTestcases++;
				try
				{
					objMan.RegisterObject(str1, -5);
					iCountErrors++;
					printerr("Error_300bb! Exception exception not thrown for negative Id");
				}
				catch (ArgumentException) {
					}catch (Exception exc){
					iCountErrors++;
					printerr("Error_300cc! Incorrect exception thrown : exc=="+exc.ToString());
				}
				strLoc = "Loc_400aa";
				str1 = "Test";
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject(str1, Int64.MaxValue);
				iCountTestcases++;
				if(!((String)objMan.GetObject(Int64.MaxValue)).Equals(str1))
				{
					iCountErrors++;
					printerr("Error_856bdf! Incorrect object returned");
				}
				strLoc = "Loc_500aa";
				str1 = "Test1";
				str2 = "Test2";
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject(str1, 5);
				try
				{
					objMan.RegisterObject(str2, 5);
					iCountErrors++;
					printerr("Error_493fh! Trying to register same ObjectID twice should throw exception");
				}
				catch (SerializationException)
				{
				}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Loc8235vdr! Incorrect Exception thrown: \n"+exc.ToString());
				}
				iCountTestcases++;
				if(!((String)objMan.GetObject(5)).Equals(str1))
				{
					iCountErrors++;
					printerr("Error_500bb! Incorrect object returned");
				}
				Int16 i16SameValue = 20;
				Int32 i32SameValue = 1310740;
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject(i16SameValue, i16SameValue.GetHashCode());
				iCountTestcases++;
				try
				{
					objMan.RegisterObject(i32SameValue, i32SameValue.GetHashCode());
				}
				catch(SerializationException){}
				catch (Exception exc)
				{
					iCountErrors++;
					printerr("Loc 634623vvdrfe! Incorrect Exception thrown: \n"+exc.ToString());
				}
				strLoc = "Loc_600aa";
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject("Test1", 1);
				objMan.RegisterObject("Test2", 2);
				objMan.RegisterObject("Test3", 3);
				objMan.RegisterObject("Test4", 4);
				iCountTestcases++;
				if(!((String)objMan.GetObject(1)).Equals("Test1"))
				{
					iCountErrors++;
					printerr("Error_600bb! Incorrect object returned");
				}
				iCountTestcases++;
				if(!((String)objMan.GetObject(2)).Equals("Test2"))
				{
					iCountErrors++;
					printerr("Error_600cc! Incorrect object returned");
				}
				iCountTestcases++;
				if(!((String)objMan.GetObject(3)).Equals("Test3"))
				{
					iCountErrors++;
					printerr("Error_600dd! Incorrect object returned");
				}
				iCountTestcases++;
				if(!((String)objMan.GetObject(4)).Equals("Test4"))
				{
					iCountErrors++;
					printerr("Error_600ee! Incorrect object returned");
				}
				strLoc = "Loc_700aa";
				Object var2 = 5;
				Decimal dec2 = (Decimal)10.5;
				Int16 i16 = 10;
				Int32 i32 = 10;
				Int64 i64 = 10;
				Single sgl2 = (Single)5.5;
				Double dbl2 = 5.5;
				objMan = new ObjectManager(isur, sc1);
				objMan.RegisterObject(var2, 1);
				objMan.RegisterObject(dec2, 2);
				objMan.RegisterObject(i16, 3);
				objMan.RegisterObject(i32, 4);
				objMan.RegisterObject(i64, 5);
				objMan.RegisterObject(sgl2, 6);
				objMan.RegisterObject(dbl2, 7);
				iCountTestcases++;
				if((int)objMan.GetObject(1) != 5)
				{
					iCountErrors++;
					printerr("Error_700bb! Incorrect value returned");
				}
				iCountTestcases++;
				if((Decimal)objMan.GetObject(2) != (Decimal)10.5)
				{
					iCountErrors++;
					printerr("Error_700cc! Incorrect Decimal value returned");
				}
				iCountTestcases++;
				if((Int16)objMan.GetObject(3) != 10)
				{
					iCountErrors++;
					printerr("Error_700dd! Incorrect Int16 value returned");
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
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
		Boolean bResult = false;
		Co5244RegisterObject_Obj_lng cbA = new Co5244RegisterObject_Obj_lng();
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
