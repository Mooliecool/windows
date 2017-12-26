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
using System.Globalization;
using System.Text;
using System.Threading;
public class Co5728EndWrite
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream.EndWrite(IASyncResult)";
    public static String s_strTFName        = "Co5728EndWrite.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        try
        {
            String filName = s_strTFAbbrev+"TestFile";
            Stream fs2;
            IAsyncResult iar;
            Byte[] bArr;
            if(File.Exists(filName)) 
                File.Delete(filName);
            strLoc = "Loc_100aa";
            fs2 = new FileStream(filName, FileMode.Create);
            iCountTestcases++;
            try 
            {
                fs2.EndWrite(null);
                iCountErrors++;
                printerr( "Error_100bb! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Info_100cc! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_100dd Incorrect exception thrown, exc=="+exc.ToString());
            }	
            fs2.Close();
            strLoc = "Loc_200aa";
            fs2 = new FileStream(filName, FileMode.Create);
            iar = fs2.BeginRead(new Byte[0], 0, 0, null, null);
            iCountTestcases++;
            try 
            {
                fs2.EndWrite(iar);
                iCountErrors++;
                printerr( "Error_200bb! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo ("Info_200cc! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200dd! Incorrect exception thrown, exc=="+exc.ToString());
            }				
            fs2.Close();
            strLoc = "Loc_300aa";
            fs2 = new FileStream(filName, FileMode.Create);
            bArr = new Byte[1024*1000];
            for(int i = 0 ; i < bArr.Length ; i++)
                bArr[i] = (Byte)i;
            iar = fs2.BeginWrite(bArr, 0, bArr.Length, null, null);
            fs2.EndWrite(iar);
            iCountTestcases++;
            if(!iar.IsCompleted) 
            {
                iCountErrors++;
                printerr( " Error_300dd! Operation should be complete");
            }
            iCountTestcases++;
            iCountTestcases++;
            if(fs2.Length != bArr.Length) 
            {
                iCountErrors++;
                printerr( "Error_300bb! Expected=="+bArr.Length+", Return=="+fs2.Length);
            }
            fs2.Close();
            strLoc = "Loc_400aa";
            fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.None, 100, true);
            bArr = new Byte[1000*1024];
            for(int i = 0 ; i < bArr.Length ; i++)
                bArr[i] = (Byte)i;
            iar = fs2.BeginWrite(bArr, 0, bArr.Length, null, null);
            fs2.EndWrite(iar);
            iCountTestcases++;
            if(!iar.IsCompleted) 
            {
                iCountErrors++;
                printerr( "Error_400bb! Operation should be complete");
            } 
            iCountTestcases++;
            iCountTestcases++;
            if(fs2.Length != bArr.Length) 
            {
                iCountErrors++;
                printerr( "Error_400dd! Expected=="+bArr.Length+", Return=="+fs2.Length);
            }
            fs2.Close();
            if(File.Exists(filName))
                File.Delete(filName);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5728EndWrite cbA = new Co5728EndWrite();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
