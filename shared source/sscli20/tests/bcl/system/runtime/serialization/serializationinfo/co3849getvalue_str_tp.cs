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
public class Co3849GetValue_str_tp
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SerializationInfo.GetValue(String, Type)";
	public static String s_strTFName        = "Co3849GetValue_str_tp.cs";
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
		String strName = null;
		Int32 iNumberOfMembers;
		String[] arrNames = null;
		Object[] arrValues = null;
		Object vntResult = null;
		try {
			do
			{
				strLoc="Loc_65739vcd";
				tpName = Type.GetType("System.Int32");
				serinfo1 = new SerializationInfo(tpName, new FormatterConverter());
				iNumberOfMembers = 50;
				arrNames = new String[iNumberOfMembers];
				arrValues = new Object[iNumberOfMembers];
				for(int i=0; i<iNumberOfMembers;i++)
				{
					arrNames[i] = i.ToString();
					arrValues[i] = i;
					serinfo1.AddValue(arrNames[i], arrValues[i], typeof(Int32));
				}
				for(int i=0; i<iNumberOfMembers;i++)
				{
					iCountTestcases++;
					if(!arrValues[i].Equals(serinfo1.GetValue(arrNames[i], typeof(Int32))))
					{
						iCountErrors++;
						Console.WriteLine("Err_0246wd_" + i.ToString() + "! Wrong value, " + arrValues[i].ToString());
					}
				}
				strLoc="Loc_7638cvd";
				tpName = Type.GetType("System.Int32");
				serinfo1 = new SerializationInfo(tpName, new FormatterConverter());
				iNumberOfMembers = 2;
				arrNames = new String[iNumberOfMembers];
				arrValues = new Object[iNumberOfMembers];
				arrNames[0] = "BlahBlah";arrValues[0] = 5;
				arrNames[1] = "BlahBlah";arrValues[1] = 10;
				serinfo1.AddValue(arrNames[0], arrValues[0], typeof(Int32));
				try{
					iCountTestcases++;
					serinfo1.AddValue(arrNames[1], arrValues[1], typeof(Int32));
					iCountErrors++;
					Console.WriteLine("Err_8346gfs! Exception not thrown");
				}catch(SerializationException){
				}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_02643vfs! Wrong Exception thrown," + ex.ToString());
				}
				strLoc="Loc_653cd";
				iCountTestcases++;
				if(!arrValues[0].Equals(serinfo1.GetValue(arrNames[0], typeof(Int32))))
				{
					iCountErrors++;
					Console.WriteLine("Err_0357vfd! Most unexpected, Expected = " + arrValues[0].ToString());
				}
				try
				{
					strName = "This is no way in the Serialziation Info";
					iCountTestcases++;
					vntResult = serinfo1.GetValue(strName, typeof(Int32));
					iCountErrors++;
					Console.WriteLine("Err_8346gfs! Exception not thrown");
				}
				catch(SerializationException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_02643vfs! Wrong Exception thrown," + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					serinfo1.GetValue(null, typeof(Int32));
					iCountErrors++;
					Console.WriteLine("Err_3765vfds! Exception not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_03246vsf! Wrong Exception thrown," + ex.ToString());
				}
				try
				{
					iCountTestcases++;
					serinfo1.GetValue("AnyString", null);
					iCountErrors++;
					Console.WriteLine("Err_76489cd! Exception not thrown");
				}
				catch(ArgumentNullException){}
				catch(Exception ex)
				{
					iCountErrors++;
					Console.WriteLine("Err_023578fed! Wrong Exception thrown," + ex.ToString());
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
		Co3849GetValue_str_tp cbA = new Co3849GetValue_str_tp();
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
