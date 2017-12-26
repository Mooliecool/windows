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
using System.Collections;
using System.Runtime.Serialization;
using System.Threading;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Serialization.Formatters.Binary;
    public class Co8501OneGraphMultipleFormatters
    {
    	public static readonly String s_strActiveBugNums = "";
    	public static readonly String s_strDtTmVer       = "";
    	public static readonly String s_strClassMethod   = "Serialize:Graph";
    	public static readonly String s_strTFName        = "Co8501OneGraphMultipleFormatters.cs";
    	public static readonly String s_strTFAbbrev      = "Co3779";
    	public static readonly String s_strTFPath        = "";
    	private MemoryStream stream;
    	private IFormatter formatter;
    	private static Graph_Object theRoot;
    	private static ArrayList syncList;
    	public virtual bool runTest()
    	{
    		const Int32 iSimpleGraphCount = 10;
			const Int32 iNumberOfThreads = 20;
    		int iCountErrors = 0;
    		int iCountTestcases = 0;
    		String strLoc="123_er";
    		Console.Out.Write( s_strClassMethod );
    		Console.Out.Write( ": " );
    		Console.Out.Write( s_strTFPath + s_strTFName );
    		Console.Out.Write( ": " );
    		Console.Out.Write( s_strDtTmVer );
    		Console.Out.WriteLine( " runTest started..." );
    		int iObjectCount;
    		Graph_Object[] oArr = new Graph_Object[iSimpleGraphCount];
    		Graph_Object DsrObj;
			ThreadStart tdst1;				
			Thread[] thdPool;
	        Co8501OneGraphMultipleFormatters[] targets;
			ArrayList list;
    		Graph_Object objCurrent;
    		Graph_Object objChild;
    		Hashtable hsh1;
    		Boolean fRepeatValue;
    		try{
    			strLoc="loc_680_sd";
    			iCountTestcases++;
    			for(int i=0; i< iSimpleGraphCount; i++)
    				oArr[i] = new Graph_Object(i);
    			iObjectCount = 0;
    			for(int i=0; i< iSimpleGraphCount; i++){
    				oArr[i].m_obj1 = oArr[++iObjectCount];
    				if(iObjectCount >= (iSimpleGraphCount-1))
    					iObjectCount=-1;
    				oArr[i].m_obj2 = oArr[++iObjectCount];
    				if(iObjectCount >= (iSimpleGraphCount-1))
    					iObjectCount=-1;
    			}    				    				
    			strLoc="loc_486af";
    			theRoot = oArr[0];
    			targets = new Co8501OneGraphMultipleFormatters[iNumberOfThreads];
    			for(int i=0; i<iNumberOfThreads;i++)
    				targets[i] = new Co8501OneGraphMultipleFormatters();
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(targets[i].DoTheSerialize);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Start();
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Join();
				list = new ArrayList();
				syncList = ArrayList.Synchronized(list);
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(targets[i].DoTheDeserialize);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Start();
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Join();
				if(syncList.Count != iNumberOfThreads){
					iCountErrors++;
					Console.WriteLine("Err_742wn! Error in the number of objects, " + syncList.Count);
				}
    			for(int loopCount = 0; loopCount<syncList.Count; loopCount++){
    				DsrObj = (Graph_Object)syncList[loopCount];
    				iObjectCount = 0;
    				objCurrent = DsrObj;
    				objChild = null;
    				strLoc="loc_015aq";
    				hsh1 = new Hashtable();
    				hsh1.Add(iObjectCount, objCurrent);
    				for(int i=1; i< iSimpleGraphCount; i++){
    					objChild = objCurrent.m_obj1;
    					hsh1.Add(++iObjectCount, objChild);
    					objChild = objCurrent.m_obj2;
    					hsh1.Add(++iObjectCount, objChild);
    					objCurrent = (Graph_Object)hsh1[i];
    				}
    				strLoc="loc_186ac";
    				iObjectCount = 0;
    				objCurrent = DsrObj;
    				objChild = null;
    				fRepeatValue = false;
    				for(int i=0; i< iSimpleGraphCount; i++){
    					strLoc="loc_639dk_" + i;
    					objCurrent = (Graph_Object)hsh1[i];
    					if (objCurrent.iValue != i){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_320ye_" + i + ", objCurrent.iValue=="+ objCurrent.iValue );
    					}
    					if (Int32.Parse(objCurrent.strValue) != i){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_932rp_" + i + ", objCurrent.strValue=="+ objCurrent.strValue );
    					}
    					strLoc="loc_275sd_" + i;
    					objChild = objCurrent.m_obj1;
    					if (objChild.iValue != ++iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_835sf_" + i + ", objChild.iValue=="+ objChild.iValue );
    					}
    					strLoc="Loc_564ds_" + i;
    					if (Int32.Parse(objChild.strValue) != iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_482xp_" + i + ", objChild.strValue=="+ objChild.strValue );
    					}
    					strLoc="loc_145nj_" + i;
    					if(fRepeatValue && (iObjectCount < iSimpleGraphCount) && (iObjectCount >= 0)){
    						if(objChild.Equals((Graph_Object)hsh1[iObjectCount]) != true){
    							++iCountErrors;
    							Console.WriteLine( s_strTFAbbrev + " Err_530dr_" + i + ", iObjectCount=="+ iObjectCount );
    						}
    					}
    					if(iObjectCount >= (iSimpleGraphCount-1)){
    						iObjectCount=-1;
    						fRepeatValue = true;
    					}
    					strLoc="loc_589fa_" + i;
    					objChild = objCurrent.m_obj2;
    					if (objChild.iValue != ++iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_735sd_" + i + ", objChild.iValue=="+ objChild.iValue );
    					}
    					if (Int32.Parse(objChild.strValue) != iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_963sx_" + i + ", objChild.strValue=="+ objChild.strValue );
    					}
    					strLoc="loc_666dt_" + i;
    					if(fRepeatValue && (iObjectCount < iSimpleGraphCount) && (iObjectCount >= 0)){
    						if(objChild.Equals((Graph_Object)hsh1[iObjectCount]) != true){
    							++iCountErrors;
    							Console.WriteLine( s_strTFAbbrev + " Err_274qm_" + i + ", iObjectCount=="+ iObjectCount );
    						}
    					}
    					if(iObjectCount >= (iSimpleGraphCount-1)){
    						iObjectCount=-1;
    						fRepeatValue = true;
    					}
    				}
    			}
    			strLoc="loc_97435gf";
    			iCountTestcases++;
    			for(int i=0; i< iSimpleGraphCount; i++)
    				oArr[i] = new Graph_Object(i);
    			iObjectCount = 0;
    			for(int i=0; i< iSimpleGraphCount; i++){
    				oArr[i].m_obj1 = oArr[++iObjectCount];
    				if(iObjectCount >= (iSimpleGraphCount-1))
    					iObjectCount=-1;
    				oArr[i].m_obj2 = oArr[++iObjectCount];
    				if(iObjectCount >= (iSimpleGraphCount-1))
    					iObjectCount=-1;
    			}    				    				
    			strLoc="loc_8732rsdg";
    			theRoot = oArr[0];
    			targets = new Co8501OneGraphMultipleFormatters[iNumberOfThreads];
    			for(int i=0; i<iNumberOfThreads;i++)
    				targets[i] = new Co8501OneGraphMultipleFormatters();
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(targets[i].DoTheSoapSerialize);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Start();
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Join();
				list = new ArrayList();
				syncList = ArrayList.Synchronized(list);
				thdPool = new Thread[iNumberOfThreads];
				for(int i=0; i<iNumberOfThreads; i++){
					tdst1 = new ThreadStart(targets[i].DoTheDeserialize);
					thdPool[i] = new Thread(tdst1);
				}
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Start();
				for(int i=0; i<iNumberOfThreads; i++)
					thdPool[i].Join();
				if(syncList.Count != iNumberOfThreads){
					iCountErrors++;
					Console.WriteLine("Err_827435sdg! Error in the number of objects, " + syncList.Count);
				}
    			for(int loopCount = 0; loopCount<syncList.Count; loopCount++){
    				DsrObj = (Graph_Object)syncList[loopCount];
    				iObjectCount = 0;
    				objCurrent = DsrObj;
    				objChild = null;
    				strLoc="loc_015aq";
    				hsh1 = new Hashtable();
    				hsh1.Add(iObjectCount, objCurrent);
    				for(int i=1; i< iSimpleGraphCount; i++){
    					objChild = objCurrent.m_obj1;
    					hsh1.Add(++iObjectCount, objChild);
    					objChild = objCurrent.m_obj2;
    					hsh1.Add(++iObjectCount, objChild);
    					objCurrent = (Graph_Object)hsh1[i];
    				}
    				strLoc="loc_186ac";
    				iObjectCount = 0;
    				objCurrent = DsrObj;
    				objChild = null;
    				fRepeatValue = false;
    				for(int i=0; i< iSimpleGraphCount; i++){
    					strLoc="loc_764wdsg_" + i;
    					objCurrent = (Graph_Object)hsh1[i];
    					if (objCurrent.iValue != i){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_9275g_" + i + ", objCurrent.iValue=="+ objCurrent.iValue );
    					}
    					if (Int32.Parse(objCurrent.strValue) != i){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_0275sg_" + i + ", objCurrent.strValue=="+ objCurrent.strValue );
    					}
    					strLoc="loc_275sd_" + i;
    					objChild = objCurrent.m_obj1;
    					if (objChild.iValue != ++iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_0752dg_" + i + ", objChild.iValue=="+ objChild.iValue );
    					}
    					strLoc="Loc_564ds_" + i;
    					if (Int32.Parse(objChild.strValue) != iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_2076wggb_" + i + ", objChild.strValue=="+ objChild.strValue );
    					}
    					strLoc="loc_276dg_" + i;
    					if(fRepeatValue && (iObjectCount < iSimpleGraphCount) && (iObjectCount >= 0)){
    						if(objChild.Equals((Graph_Object)hsh1[iObjectCount]) != true){
    							++iCountErrors;
    							Console.WriteLine( s_strTFAbbrev + " Err_92721455g_" + i + ", iObjectCount=="+ iObjectCount );
    						}
    					}
    					if(iObjectCount >= (iSimpleGraphCount-1)){
    						iObjectCount=-1;
    						fRepeatValue = true;
    					}
    					strLoc="loc_589fa_" + i;
    					objChild = objCurrent.m_obj2;
    					if (objChild.iValue != ++iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_2975sg_" + i + ", objChild.iValue=="+ objChild.iValue );
    					}
    					if (Int32.Parse(objChild.strValue) != iObjectCount){
    						++iCountErrors;
    						Console.WriteLine( s_strTFAbbrev + " Err_8724fdg_" + i + ", objChild.strValue=="+ objChild.strValue );
    					}
    					strLoc="loc_666dt_" + i;
    					if(fRepeatValue && (iObjectCount < iSimpleGraphCount) && (iObjectCount >= 0)){
    						if(objChild.Equals((Graph_Object)hsh1[iObjectCount]) != true){
    							++iCountErrors;
    							Console.WriteLine( s_strTFAbbrev + " Err_8724sbg_" + i + ", iObjectCount=="+ iObjectCount );
    						}
    					}
    					if(iObjectCount >= (iSimpleGraphCount-1)){
    						iObjectCount=-1;
    						fRepeatValue = true;
    					}
    				}
    			}
    		}
    		catch (Exception exc_general){
    			++iCountErrors;
    			Console.WriteLine( s_strTFAbbrev + "Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
    		}
    		if ( iCountErrors == 0 )
    		{
    			Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
    			return true;
    		}
    		else
    		{
    			Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
    			return false;
    		}
    	}
		private void DoTheSerialize(){
			stream = new MemoryStream();
			formatter = new BinaryFormatter();
			formatter.Serialize(stream, theRoot);
		}
		private void DoTheSoapSerialize(){
			stream = new MemoryStream();
			formatter = new SoapFormatter();
			formatter.Serialize(stream, theRoot);
		}
		private void DoTheDeserialize(){
			stream.Position = 0;
			syncList.Add(formatter.Deserialize(stream));
		}
    	public static void Main( String[] args ){
    		bool bResult = false;	
    		Co8501OneGraphMultipleFormatters oCbTest = new Co8501OneGraphMultipleFormatters();
    		try
    		{
    			bResult = oCbTest.runTest();
    		}
    		catch ( Exception exc_main )
    		{
    			bResult = false;
    			Console.WriteLine( s_strTFAbbrev + "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
    		}
    		if ( ! bResult )
    		{
    			Console.WriteLine( s_strTFAbbrev + s_strTFPath );
    			Console.Error.WriteLine( " " );
    			Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
    			Console.Error.WriteLine( " " );
    		}
    		if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    	}
    }
[Serializable]
internal class Graph_Object
{
	public int iValue;
	public String strValue;
	public Graph_Object m_obj1;
	public Graph_Object m_obj2;
	internal Graph_Object(int value){
		iValue = value;
		strValue = (((Int32)value)).ToString();
	}
}
