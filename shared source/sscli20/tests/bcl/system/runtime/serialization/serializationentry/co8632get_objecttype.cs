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
public class Co8632get_ObjectType
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SerializationEntry.ObjectType";
	public static String s_strTFName        = "Co8632get_ObjectType.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		SerializationInfo serinfo1 = null;
		Type tpName = null;
		Int32 iNumberOfMembers;
		String[] arrNames = null;
		Object[] arrValues = null;
		SerializationInfoEnumerator serenum1 = null;
		int iNum = 0;
		SerializationEntry serEnt1;
		try {
			do
			{
				serinfo1 = new SerializationInfo(typeof(Int32), new FormatterConverter());
				iNumberOfMembers = 50;
				arrNames = new String[iNumberOfMembers];
				arrValues = new Object[iNumberOfMembers];
				for(int i=0; i<iNumberOfMembers;i++)
				{
					arrNames[i] = i.ToString();
					arrValues[i] = i;
					serinfo1.AddValue(arrNames[i], arrValues[i], typeof(Int32));
				}
				serenum1 = serinfo1.GetEnumerator();
				iNum = 0;
				while(serenum1.MoveNext())
				{
					iCountTestcases++;
					serEnt1 = serenum1.Current;
					if(!serEnt1.ObjectType.Equals(typeof(Int32)))
					{
						iCountErrors++;
						Console.WriteLine("Err_4753fg_" + iNum.ToString() + "! Wrong value, " + serEnt1.ObjectType);
					}
					if(!serEnt1.Name.Equals(iNum.ToString()))
					{
						iCountErrors++;
						Console.WriteLine("Err_7439gdg_" + iNum.ToString() + "! Wrong value, " + serEnt1.Name);
					}
					if((Int32)serEnt1.Value != iNum)
					{
						iCountErrors++;
						Console.WriteLine("Err_7439gdg_" + iNum.ToString() + "! Wrong value, " + serEnt1.Value);
					}
					iNum++;
				}
				tpName = Type.GetType("System.Int16");
				serinfo1 = new SerializationInfo(tpName, new FormatterConverter());
				serenum1 = serinfo1.GetEnumerator();
				iCountTestcases++;
				if(serenum1.MoveNext())
				{
					iCountErrors++;
					Console.WriteLine("Err_03576bfd! most peculiar, MoveNext returned true!");
				}
				try
				{
					iCountTestcases++;
					serEnt1 = serenum1.Current;
					iCountErrors++;
					Console.WriteLine("Err_0346gsdf! Exception not thrown");
				}
				catch(InvalidOperationException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_752gs! Wrong Exception thrown," + ex.ToString());
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
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
		Co8632get_ObjectType cbA = new Co8632get_ObjectType();
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
