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
using System.IO;
public class Co3965StreamMethods
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "Stream: ";
	public static String s_strTFName        = "Co3965StreamMethods.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		MemoryStream ms1;
		FileStream fs1;
		BufferedStream bs1;
		BufferedStream bs2;
		BufferedStream bsPrevious;
		BufferedStream[] bsArr;
		Int32 iNumberOfStreams = 50;
		try {
			do
			{
				strLoc = "Loc_573dsf";
				iCountTestcases++;
				ms1 = new MemoryStream();
				iCountTestcases++;
				if(!SeekTest(ms1, false)){
					iCountErrors++;
					Console.WriteLine("Err_742df! Expected value not returned");
				}
				fs1 = new FileStream("test.txt", FileMode.Create);
				iCountTestcases++;
				if(!SeekTest(fs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_7542dsg! Expected value not returned");
				}
				ms1 = new MemoryStream(new Byte[1024], false);
				iCountTestcases++;
				if(!SeekTest(ms1, false)){
					iCountErrors++;
					Console.WriteLine("Err_52w4dsv! Expected value not returned");
				}
				bs1 = new BufferedStream(ms1);
				iCountTestcases++;
				if(!SeekTest(bs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_573fds! Expected value not returned");
				}
				bs2 = new BufferedStream(bs1);
				iCountTestcases++;
				if(!SeekTest(bs2, true)){
					iCountErrors++;
					Console.WriteLine("Err_0752fdg! Expected value not returned");
				}
				bsArr = new BufferedStream[iNumberOfStreams];
				bsPrevious = bs2;
				for(int i=0; i<iNumberOfStreams; i++){
					bsArr[i] = new BufferedStream(bsPrevious);
					if(!SeekTest(bsArr[i], true)){
						iCountErrors++;
						Console.WriteLine("Err_2974sff_" + i + "! Expected value not returned");
					}
					bsPrevious = bsArr[i];
				}
				bs1 = new BufferedStream(fs1);
				iCountTestcases++;
				if(!SeekTest(bs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_520nxf! Expected value not returned");
				}				
				bs2 = new BufferedStream(bs1);
				iCountTestcases++;
				if(!SeekTest(bs2, true)){
					iCountErrors++;
					Console.WriteLine("Err_97423dsfd! Expected value not returned");
				}
				bsArr = new BufferedStream[iNumberOfStreams];
				bsPrevious = bs2;
				for(int i=0; i<iNumberOfStreams; i++){
					bsArr[i] = new BufferedStream(bsPrevious);
					if(!SeekTest(bsArr[i], true)){
						iCountErrors++;
						Console.WriteLine("Err_2974sff_" + i + "! Expected value not returned");
					}
					bsPrevious = bsArr[i];
				}
				for(int i=iNumberOfStreams-1; i>=0; i--)
					bsArr[i].Close();
				bs2.Close();
				bs1.Close();
				fs1.Close();
				fs1 = new FileStream("test.txt", FileMode.Open, FileAccess.Read, FileShare.None);
				iCountTestcases++;
				if(!SeekTest(fs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_4529ad! Expected value not returned");
				}
				bs1 = new BufferedStream(fs1);
				if(!SeekTest(bs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_57fdsg! Expected value not returned");
				}
				bs2 = new BufferedStream(bs1);
				if(!SeekTest(bs2, true)){
					iCountErrors++;
					Console.WriteLine("Err_2748qn! Expected value not returned");
				}
				bsArr = new BufferedStream[iNumberOfStreams];
				bsPrevious = bs2;
				for(int i=0; i<iNumberOfStreams; i++){
					bsArr[i] = new BufferedStream(bsPrevious);
					if(!SeekTest(bsArr[i], true)){
						iCountErrors++;
						Console.WriteLine("Err_2974sff_" + i + "! Expected value not returned");
					}
					bsPrevious = bsArr[i];
				}
				for(int i=iNumberOfStreams-1; i>=0; i--)
					bsArr[i].Close();
				bs2.Close();
				bs1.Close();
				fs1.Close();
				File.Delete("test.txt");
				Console.WriteLine();
				iCountTestcases++;
				ms1 = new MemoryStream();
				if(!StreamTest(ms1, false)){
					iCountErrors++;
					Console.WriteLine("Err_7439fds! Expected value not returned");
				}
				bs1 = new BufferedStream(ms1);
				if(!StreamTest(bs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_74329dfsf! Expected value not returned");
				}
				bs2 = new BufferedStream(bs1);
				if(!StreamTest(bs2, true)){
					iCountErrors++;
					Console.WriteLine("Err_0427fdg! Expected value not returned");
				}
				bsArr = new BufferedStream[iNumberOfStreams];
				bsPrevious = bs2;
				for(int i=0; i<iNumberOfStreams; i++){
					bsArr[i] = new BufferedStream(bsPrevious);
					if(!StreamTest(bsArr[i], true)){
						iCountErrors++;
						Console.WriteLine("Err_0278swdf_" + i + "! Expected value not returned");
					}
					bsPrevious = bsArr[i];
				}
				for(int i=iNumberOfStreams-1; i>=0; i--)
					bsArr[i].Close();
				fs1 = new FileStream("test.dat", FileMode.Create);
				if(!StreamTest(fs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_74532dsg! Expected value not returned");
				}
				bs1 = new BufferedStream(fs1);
				if(!StreamTest(bs1, false)){
					iCountErrors++;
					Console.WriteLine("Err_742dg! Expected value not returned");
				}
				bs2 = new BufferedStream(bs1);
				if(!StreamTest(bs2, true)){
					iCountErrors++;
					Console.WriteLine("Err_0743dsf! Expected value not returned");
				}
				bsArr = new BufferedStream[iNumberOfStreams];
				bsPrevious = bs2;
				for(int i=0; i<iNumberOfStreams; i++){
					bsArr[i] = new BufferedStream(bsPrevious);
					if(!StreamTest(bsArr[i], true)){
						iCountErrors++;
						Console.WriteLine("Err_0278swdf_" + i + "! Expected value not returned");
					}
					bsPrevious = bsArr[i];
				}
				for(int i=iNumberOfStreams-1; i>=0; i--)
					bsArr[i].Close();
				bs2.Close();
				bs1.Close();
				fs1.Close();
				File.Delete("test.dat");
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
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
	private Boolean SeekTest(Stream stream, Boolean fSuppres){
		long lngPos;
		Byte btValue;
		String strRead  = "Read-Only";
		if(stream.CanWrite)
		strRead  = "Read-Write";
		if(!fSuppres)
			Console.WriteLine("Testing " + stream.GetType() + " " + strRead);
		stream.Position = 0;
		if(stream.Position != 0)
			return false;
		Int32 length = 1 << 10 ; 
		Byte[] btArr = new Byte[length];
		for(int i=0; i<btArr.Length; i++)
			btArr[i] = (byte)i;
		if(stream.CanWrite)
			stream.Write(btArr, 0, btArr.Length);
		else
			stream.Position = btArr.Length;
		if(stream.Position != btArr.Length)
			return false;
		lngPos = stream.Seek(0, SeekOrigin.Begin);
		if(lngPos!=0){
			return false;
		}
		if(stream.Position != 0)
			return false;
		for(int i=0; i<btArr.Length; i++){
			if(stream.CanWrite){
				btValue = (Byte)stream.ReadByte();
				if(btArr[i]!=btValue)
					return false;
			}else{
				stream.Seek(1, SeekOrigin.Current);
			}
			if(stream.Position != (i+1))
				return false;
		}
		try{
			lngPos = stream.Seek(-5, SeekOrigin.Begin);
			return false;
		}catch(IOException){
		}
		try{
			lngPos = stream.Seek(btArr.Length+1, SeekOrigin.Begin);
			if( lngPos == btArr.Length + 1 )
                return true;
            else
                return false;
		}catch(IOException){
		}
		lngPos = stream.Seek(5, SeekOrigin.Begin);
		if(lngPos!=5){
			return false;
		}
		if(stream.Position != 5)
			return false;
		try{
			lngPos = stream.Seek(5, SeekOrigin.End);
			return false;
		}catch(IOException){
		}
		try{
			lngPos = stream.Seek(-(btArr.Length+1), SeekOrigin.End);
			return false;
		}catch(IOException){
		}
		lngPos = stream.Seek(-5, SeekOrigin.End);
		if(lngPos!=(btArr.Length-5)){
			return false;
		}
		if(stream.Position != (btArr.Length-5)){
			return false;
        }
		lngPos = stream.Seek(0, SeekOrigin.End);
		if(stream.Position != btArr.Length)
			return false;
		for(int i=btArr.Length; i>0; i--){
			stream.Seek(-1, SeekOrigin.Current);
			if(stream.Position != (i-1)){
				return false;
            }
		}
		try{
			lngPos = stream.Seek(-1, SeekOrigin.Current);
			return false;
		}catch(IOException){
		}
		return true;
	}
	private Boolean StreamTest(Stream stream, Boolean fSuppress){
		if(!fSuppress)
			Console.WriteLine("Testing " + stream.GetType() + " for read/write tests");
		String strValue;
		Int32 iValue;
		Int32 iLength = 1 << 10;
		stream.Seek(0, SeekOrigin.Begin);
		for(int i=0; i<iLength; i++)
			stream.WriteByte((Byte)i);
		Byte[] btArr = new Byte[iLength];
		for(int i=0; i<iLength; i++)
			btArr[i] = (Byte)i;
		stream.Write(btArr, 0, iLength);
		BinaryWriter bw1 = new BinaryWriter(stream);
		bw1.Write(false);
		bw1.Write(true);
		for(int i=0; i<10; i++){
			bw1.Write((Byte)i);
			bw1.Write((SByte)i);
			bw1.Write((Int16)i);
			bw1.Write((Char)i);
			bw1.Write((UInt16)i);
			bw1.Write(i);
			bw1.Write((UInt32)i);
			bw1.Write((Int64)i);
			bw1.Write((UInt64)i);
			bw1.Write((Single)i);
			bw1.Write((Double)i);
		}
		Char[] chArr = new Char[iLength];
		for(int i=0; i<iLength;i++)
			chArr[i] = (Char)i;
		bw1.Write(chArr);
		bw1.Write(chArr, 512, 512);
		bw1.Write(new String(chArr));
		bw1.Write(new String(chArr));
		stream.Seek(0, SeekOrigin.Begin);
		for(int i=0; i<iLength; i++){
			if(stream.ReadByte() != i%256){
                return false;
            }
		}
		btArr = new Byte[iLength];
		stream.Read(btArr, 0, iLength);
		for(int i=0; i<iLength; i++){
			if(btArr[i] != (Byte)i){
				Console.WriteLine(i + " "  + btArr[i] + " " + (Byte)i);
				return false;
			}
		}
		BinaryReader br1 = new BinaryReader(stream);
		if(br1.ReadBoolean())
			return false;
		if(!br1.ReadBoolean())
			return false;
		for(int i=0; i<10; i++){
			if(br1.ReadByte() != (Byte)i)
			return false;
			if(br1.ReadSByte() != (SByte)i)
			return false;
			if(br1.ReadInt16() != (Int16)i)
			return false;
			if(br1.ReadChar() != (Char)i)
			return false;
			if(br1.ReadUInt16() != (UInt16)i)
			return false;
			if(br1.ReadInt32() != i)
			return false;
			if(br1.ReadUInt32() != (UInt32)i)
			return false;
			if(br1.ReadInt64() != (Int64)i)
			return false;
			if(br1.ReadUInt64() != (UInt64)i)
			return false;
			if(br1.ReadSingle() != (Single)i)
			return false;
			if(br1.ReadDouble() != (Double)i)
			return false;
		}
		chArr = br1.ReadChars(iLength);
		for(int i=0; i<iLength;i++){
			if(chArr[i] != (Char)i)
			return false;
		}
		chArr = new Char[512];
		chArr = br1.ReadChars(iLength/2);
		for(int i=0; i<iLength/2;i++){
			if(chArr[i] != (Char)(iLength/2+i))
			return false;
		}
		chArr = new Char[iLength];
		for(int i=0; i<iLength;i++)
			chArr[i] = (Char)i;
		strValue = br1.ReadString();
		if(!strValue.Equals(new String(chArr)))
			return false;
		strValue = br1.ReadString();
		if(!strValue.Equals(new String(chArr))){
            return false;
        }
		try{
			stream.Seek(1, SeekOrigin.Current);
			return true;
			}catch(Exception){
		}
		stream.Position =  0;
		btArr = new Byte[iLength];
		for(int i=0; i<iLength; i++)
			btArr[i] = (Byte)(i + 5);
		AsyncCallback acb1 = new AsyncCallback(new Co3965StreamMethods().AsyncTestCB);
		IAsyncResult isync1 = stream.BeginWrite(btArr, 0, btArr.Length, acb1, stream.GetType().ToString());
		stream.EndWrite(isync1);
		stream.Position = 0;
		for(int i=0; i<iLength; i++){
			if(stream.ReadByte() != (Byte)(i+5))
			return false;
		}
		stream.Position = 0;
		AsyncCallback acb2 = new AsyncCallback(new Co3965StreamMethods().AsyncTestCB);
		Byte[] btArr1 = new Byte[iLength];
		IAsyncResult isync2 = stream.BeginRead(btArr1, 0, btArr1.Length, acb2, stream.GetType().ToString());
		iValue = stream.EndRead(isync2);
		if(iValue!=btArr.Length)
			return false;
		for(int i=0; i<iLength; i++){
			if(btArr[i] != btArr1[i])
				return false;
		}
		return true;
	}
	public void AsyncTestCB(IAsyncResult iar){
		if(!iar.IsCompleted)
			throw new Exception("Loc_7429cdf! We dont like this answer, so there!");
		if(iar.CompletedSynchronously)
			throw new Exception("Loc_832dsf! We dont like this answer, so there!");
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co3965StreamMethods cbA = new Co3965StreamMethods();
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
