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
using System.Threading;
public class Co8559Enum
{
	public static readonly String s_strClassMethod   = "Enum - threadsafety test";
	public static readonly String s_strTFName        = "Co8559Enum.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	public virtual bool runTest()
	{
		const int numberOfThreads = 20;
		Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ThreadStart tdst1;				
		Thread[] thdPool;
		ThreadStart tdst2;
		Thread[] thdPool2;
		try {
			strLoc = "Loc_001";
			iCountTestcases++;
			strLoc = "Loc_002";
			iCountTestcases++;
			thdPool = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst1 = new ThreadStart(this.WorkOnStaticMethods);
				thdPool[i] = new Thread(tdst1);
			}
			thdPool2 = new Thread[numberOfThreads];
			for(int i=0; i<numberOfThreads; i++){
				tdst2 = new ThreadStart(this.WorkOnStaticMethods);
				thdPool2[i] = new Thread(tdst2);
			}
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Start();
			for(int i=0; i<numberOfThreads; i++)
				thdPool[i].Join();
			for(int i=0; i<numberOfThreads; i++)
				thdPool2[i].Join();
		} catch (Exception exc_general ) {
		   ++iCountErrors;
		   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
		}
        if ( iCountErrors == 0 ){
           Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
           return true;
        }
        else{
           Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors);
           return false;
        }
    }
	private void WorkOnStaticMethods()
	{
		String str1 = Enum.Format(typeof(E), E.ONE, "G");
		if(str1 != "ONE")
			throw new Exception("Err_93247sDFG! wrong value returned");
		if(Enum.Format(typeof(E_L), E_L.MINUSONE, "G") != "MINUSONE")
			throw new Exception("Err_3247tsg! wrong value returned");
		if(Enum.Format(typeof(E_L), E_L.MINUSONE, "G") != "MINUSONE")
			throw new Exception("Err_3247tsg! wrong value returned");
		if(Enum.Format(typeof(E_L), E_L.MINUSONE, "X") != "FFFFFFFFFFFFFFFF")
			throw new Exception("Err_3247tsg! wrong value returned");
		if(Enum.Format(typeof(E_L), E_L.MINUSONE, "d") != "-1")
			throw new Exception("Err_3247tsg! wrong value returned");
		if(Enum.Format(typeof(E_F), E_F.ONE|E_F.FOUR, "f") != "ONE, FOUR")
			throw new Exception("Err_349t7sfg! wrong value returned");
		if(Enum.GetName(typeof(E_L), E_L.ONE) != "ONE")
			throw new Exception("Err_349t7sfg! wrong value returned");
		String[] values = Enum.GetNames(typeof(E_L));
		if(values.Length!=3
		|| values[0] != "ZERO"
		|| values[1] != "ONE"
		|| values[2] != "MINUSONE"
		)
			throw new Exception("Err_307gfgb! wrong value returned");
		if(!Enum.GetUnderlyingType(typeof(E_L)).Equals(typeof(Int64)))
			throw new Exception("Err_349t7sfg! wrong value returned");
		E_L[] ls = (E_L[])Enum.GetValues(typeof(E_L));
		if(ls.Length!=3
		|| ls[0] != E_L.ZERO
		|| ls[1] != E_L.ONE
		|| ls[2] != E_L.MINUSONE
		)
			throw new Exception("Err_3094tsg! wrong value returned");
		if(!Enum.IsDefined(typeof(E_L), E_L.ONE))
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_L)Enum.Parse(typeof(E_L), "ONE") != E_L.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_L)Enum.Parse(typeof(E_L), "minusone", true) != E_L.MINUSONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_L)Enum.ToObject(typeof(E_L), 1L) != E_L.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_S)Enum.ToObject(typeof(E_S), 1) != E_S.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_B)Enum.ToObject(typeof(E_B), 1) != E_B.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E)Enum.ToObject(typeof(E), 1) != E.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_R)Enum.ToObject(typeof(E_R), 1) != E_R.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_U)Enum.ToObject(typeof(E_U), 1) != E_U.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_V)Enum.ToObject(typeof(E_V), 1) != E_V.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
		if((E_W)Enum.ToObject(typeof(E_W), 1ul) != E_W.ONE)
			throw new Exception("Err_349t7sfg! wrong value returned");
	}
	static void MyHandler(object sender, UnhandledExceptionEventArgs args) 
	{
		Exception e = (Exception) args.ExceptionObject;
		Console.WriteLine("MyHandler caught : " + e.Message);
		Environment.Exit(1);
	}
	public static void Main(String[] args)
	{
		AppDomain currentDomain = AppDomain.CurrentDomain;
		currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyHandler);
		bool bResult = false;
	   Co8559Enum cbA = new Co8559Enum();
	   try {
	      bResult = cbA.runTest();
	   } catch (Exception exc_main){
	      bResult = false;
	      Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
	   }
	   if (!bResult)
	   {
	      Console.WriteLine(s_strTFName+ s_strTFPath);
	      Console.Error.WriteLine( " " );
	      Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
	      Console.Error.WriteLine( " " );
	   }
	   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}	
}
public enum E
{
	MINUSONE = -1,
	ZERO = 0,
	ONE=1,
}
public enum E_L:long
{
	MINUSONE = -1,
	ZERO = 0,
	ONE=1,
}
public enum E_R:short
{
	MINUSONE = -1,
	ZERO = 0,
	ONE=1,
}
public enum E_S:sbyte
{
	MINUSONE = -1,
	ZERO = 0,
	ONE=1,
}
public enum E_B:byte
{
	ZERO = 0,
	ONE=1,
}
public enum E_U:uint
{
	ZERO = 0,
	ONE=1,
}
public enum E_V:ushort
{
	ZERO = 0,
	ONE=1,
}
public enum E_W:ulong
{
	ZERO = 0,
	ONE=1,
}
[Flags]
public enum E_F
{
	ZERO = 0,
	ONE=1,
	TWO=2,
	FOUR=4,
}
