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
using System.Runtime.Serialization;
using System.IO;
using System.Reflection;
using System.Text;
public class Co3858GetObjectData
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "FormatterServices.GetObjectData(Object, MemberInfo[])";
	public static String s_strTFName        = "Co3858GetObjectData.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		Console.WriteLine();
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ConstructorInfo[] cons = null;
		Object[] arrVntInitialValues;
		Int32 iValue;
		Type tpName = null;
		MemberInfo[] members = null;
		Object[] arrVntData = null;
		Object objValue = null;
		baloney bal1 = null;
		Int16 shtValue;
		bool fCreatedSuccesfully;
		StringBuilder sbldr1 = null;
		Int32 iCheck;
		Module mdClassLib = null;
		Type[] arrTpClassLib = null;
		Type[] arrTpPopularValutTypes = {typeof(Int16), typeof(Int32), typeof(Int64),
			typeof(UInt16), typeof(UInt32), typeof(UInt64),
			typeof(SByte), typeof(Byte),
			typeof(Double), typeof(Single)
		};
		try {
			do
			{
				strLoc = "Loc_435cd";
				iValue = 10;
				tpName = iValue.GetType();
				members = FormatterServices.GetSerializableMembers(tpName);
				arrVntData = FormatterServices.GetObjectData(iValue, members);
				iCountTestcases++;
				if(arrVntData.Length!=1)
				{
					iCountErrors++;
					Console.WriteLine("Err_743cd! Int32 has more members than before, " + arrVntData.Length.ToString());
				}
				iCountTestcases++;
				if((int)arrVntData[0] != iValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_4692fd! wrong value returned, " + arrVntData[0].ToString());
				}
				for(int i=0; i<arrTpPopularValutTypes.Length;i++)
				{
					strLoc = "Loc_0467cvd_" + i.ToString();
					arrVntInitialValues = new Object[1];
					arrVntInitialValues[0] = (i+10).ToString();
					objValue = arrTpPopularValutTypes[i].InvokeMember("Parse", BindingFlags.InvokeMethod, null, null, arrVntInitialValues);
					members = FormatterServices.GetSerializableMembers(arrTpPopularValutTypes[i]);
					arrVntData = FormatterServices.GetObjectData(objValue, members);
					iCountTestcases++;
					if(arrVntData.Length!=1)
					{
						iCountErrors++;
						Console.WriteLine("Err_863xw! " + arrTpPopularValutTypes[i].Name + " has more members than before, " + arrVntData.Length.ToString());
					}
					iCheck = ((IConvertible)arrVntData[0]).ToInt32(null);
					iCountTestcases++;
					if(iCheck != (i+10))
					{
						iCountErrors++;
						Console.WriteLine("Err_395fd! " + arrTpPopularValutTypes[i].Name + " member value is different than before, " + arrVntData[0].ToString());
					}
				}
				strLoc = "Loc_02547cs";
				mdClassLib = typeof(String).Module;
				arrTpClassLib = mdClassLib.GetTypes();
				for(int i=0; i<arrTpClassLib.Length;i++)
				{
					if(!arrTpClassLib[i].IsPublic || !arrTpClassLib[i].IsSerializable)
					continue;
					try
					{
						members = FormatterServices.GetSerializableMembers(arrTpClassLib[i]);
						fCreatedSuccesfully = false;
						try {
							cons = arrTpClassLib[i].GetConstructors();
							for(int j=0; j<cons.Length; j++){
								if(cons[j].IsPublic && cons[j].GetParameters().Length==0) {
									objValue = Activator.CreateInstance(arrTpClassLib[i]);
									fCreatedSuccesfully=true;
									break;
								}
							}
							}catch(Exception){fCreatedSuccesfully=false;}
							if(fCreatedSuccesfully){
								iCountTestcases++;
								arrVntData = FormatterServices.GetObjectData(objValue, members);
							}
						}
						catch(Exception ex)
						{
							iCountErrors++;
							Console.WriteLine("Err_846qm! Unexpected exception thrown for the type, " + arrTpClassLib[i].FullName + ", " + ex.ToString());
						}
					}
					try
					{
						iCountTestcases++;
						arrVntData = FormatterServices.GetObjectData(null, members);
						iCountErrors++;
						Console.WriteLine("Err_935vd! We were expecting this to throw!");
					}
					catch(ArgumentNullException){}
					catch(Exception ex)
					{
						iCountErrors++;
						Console.WriteLine("Err_935fd! Unexpected exception thrown, " + ex.ToString());
					}
					try
					{
						iCountTestcases++;
						arrVntData = FormatterServices.GetObjectData(new Object(), null);
						iCountErrors++;
						Console.WriteLine("Err_106vs! We were expecting this to throw!");
					}
					catch(ArgumentNullException){}
					catch(Exception ex)
					{
						iCountErrors++;
						Console.WriteLine("Err_720cxa! Unexpected exception thrown, " + ex.ToString());
					}
					strLoc = "Loc_639vf";
					iValue = 20;
					shtValue = 30;
					members = FormatterServices.GetSerializableMembers(shtValue.GetType());
					try{
						arrVntData = FormatterServices.GetObjectData(iValue, members);
					}
					catch(ArgumentException){}
					catch(Exception ex)
					{
						iCountErrors++;
						Console.WriteLine("Err_934cs! Wrong Exception thrown, " + ex.ToString());
					}
					strLoc = "Loc_0256fd";
					iValue = 20;
					sbldr1 = new StringBuilder("Hello");
					members = FormatterServices.GetSerializableMembers(sbldr1.GetType());
					try
					{
						iCountTestcases++;
						arrVntData = FormatterServices.GetObjectData(iValue, members);
						iCountErrors++;
						Console.WriteLine("Err_9357vs! We were expecting this to throw!");
					}
					catch(ArgumentException){}
					catch(Exception ex)
					{
						iCountErrors++;
						Console.WriteLine("Err_934cs! Wrong Exception thrown, " + ex.ToString());
					}
					bal1 = new baloney();
					members = FormatterServices.GetSerializableMembers(bal1.GetType());
					arrVntData = FormatterServices.GetObjectData(bal1, members);
					iCountTestcases++;
					if(arrVntData.Length!=2)
					{
						iCountErrors++;
						Console.WriteLine("Err_9e5fd! baloney has more members than before, " + arrVntData.Length.ToString());
					}
					iCountTestcases++;
					if(!arrVntData[0].ToString().Equals("Hello World"))
					{
						iCountErrors++;
						Console.WriteLine("Err_036cs! baloney's member value is different than before, " + arrVntData[0].ToString());
					}
					iCountTestcases++;
					if((int)arrVntData[1] != 20)
					{
						iCountErrors++;
						Console.WriteLine("Err_302xs! baloney's member value is different than before, " + arrVntData[1].ToString());
					}
				} while (false);
				} catch (Exception exc_general ) {
				++iCountErrors;
				Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general);
			}
			if ( iCountErrors == 0 )
			{
				Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
				return true;
			}
			else
			{
				Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
				return false;
			}
		}
		public static void Main(String[] args)
		{
			bool bResult = false;
			Co3858GetObjectData cbA = new Co3858GetObjectData();
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
			if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
		}
	}
	[Serializable] public class baloney
	{
		internal String str_Humpty;
		public int i_spy;
		public baloney()
		{
			str_Humpty = "Hello World";
			i_spy = 20;
		}
	}
