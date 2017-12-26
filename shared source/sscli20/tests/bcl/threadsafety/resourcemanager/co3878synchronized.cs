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
using System.Threading;
using System.Resources;
using System.IO;
using System.Reflection;
interface IDescribeTestedMethods
{
	MemberInfo[] GetTestedMethods();
}
public class Co3878Synchronized : IDescribeTestedMethods
{
	public MemberInfo[] GetTestedMethods()
	{
		Type type = typeof(ResourceManager);
		MethodInfo[] methods = new MethodInfo[0];
		return methods;
	}
	public static readonly String s_strActiveBugNums = "";
	public static readonly String s_strDtTmVer       = "";
	public static readonly String s_strClassMethod   = "ResourceManager.Sync test";
	public static readonly String s_strTFName        = "Co3878Synchronized.cs";
	public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static readonly String s_strTFPath        = Environment.CurrentDirectory;
	internal ResourceManager manager;
	internal const Int32 iNumberOfThreads = 100;
	internal const Int32 iNumberOfStringsInFile = 200;
	internal static Int32 iThreadsCompleted = 0;
	public virtual bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		ResourceWriter writer;
		ThreadStart tdst1;
		Thread[] thdPool;
		Boolean fLoopExit;
		try {
			do
			{
				iCountTestcases++;
				if(File.Exists(Environment.CurrentDirectory+"\\Co3878_A.resources"))
				File.Delete(Environment.CurrentDirectory+"\\Co3878_A.resources");
				writer = new ResourceWriter("Co3878_A.resources");
				for(int i=0; i<iNumberOfStringsInFile; i++)
				writer.AddResource("Key " + i, "Value " + i);
				writer.Generate();
				writer.Close();
				iThreadsCompleted = 0;
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(this.RSMangerForEachThread);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++){
					thdPool[i].Start();
				}
				do {
                                        int numberAlive = 0;
					fLoopExit = false;
                                        Thread.Sleep(100);
					for(int i=0; i<iNumberOfThreads; i++){
						if(thdPool[i].IsAlive) {
						fLoopExit = true; 
                                                numberAlive ++; 
                                                }
					}
				}while(fLoopExit);
				if(iThreadsCompleted != iNumberOfThreads){
					iCountErrors++;
					Console.WriteLine("Err_67423csd! All the thrads didn't execute the function. Expected, " + iNumberOfThreads + " completed, " + iThreadsCompleted);
				}
				iCountTestcases++;
				if(File.Exists(Environment.CurrentDirectory+"\\Co3878_B.resources"))
				File.Delete(Environment.CurrentDirectory+"\\Co3878_B.resources");
				writer = new ResourceWriter("Co3878_B.resources");
				for(int i=0; i<iNumberOfStringsInFile; i++)
				writer.AddResource("Key " + i, "Value " + i);
				writer.Generate();
				writer.Close();
				manager = ResourceManager.CreateFileBasedResourceManager("Co3878_B", Environment.CurrentDirectory, null);
				iThreadsCompleted = 0;
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(this.RSMangerForAllThreads);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++){
					thdPool[i].Start();
				}
				do {
					fLoopExit = false;
                                        Thread.Sleep(100);
					for(int i=0; i<iNumberOfThreads; i++){
						if(thdPool[i].IsAlive)
						fLoopExit = true;
					}
				}while(fLoopExit);
				if(iThreadsCompleted != iNumberOfThreads){
					iCountErrors++;
					Console.WriteLine("Err_7539cd! All the threads didn't execute the function. Expected, " + iNumberOfThreads + " completed, " + iThreadsCompleted);
				}
				manager.ReleaseAllResources();
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
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
	private void RSMangerForEachThread()
	{
		ResourceManager managerLocal = 
                        ResourceManager.CreateFileBasedResourceManager("Co3878_A", Environment.CurrentDirectory, null);
		String strKey;
		String strValue;
		for(int i=0; i<iNumberOfStringsInFile; i++) {
			strKey = "Key " + i;
			strValue = "Value " + i;
			if(!managerLocal.GetString(strKey).Equals(strValue)) {
				throw new Exception(manager.GetString(strKey) + " " + strValue);
			}
		}
		managerLocal.ReleaseAllResources();
		lock (this) { iThreadsCompleted++; }
	}
	private void RSMangerForAllThreads()
	{
		String strKey;
		String strValue;
		for(int i = 0; i<iNumberOfStringsInFile; i++) {
			strKey = "Key " + i;
			strValue = "Value " + i;
			if(!manager.GetString(strKey).Equals(strValue)) {
				throw new Exception(manager.GetString(strKey) + " " + strValue);
			}
		}
		lock (this) { iThreadsCompleted++; }
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
		Co3878Synchronized cbA = new Co3878Synchronized();
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
		if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
	}
}
