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
public class Co5575ctor_str_fm
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Copy(String)";
    public static String s_strTFName        = "Co5575ctor_str_fm.cs";
    public static String s_strTFAbbrev      = "Co5575";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            FileStream fs2;
            StreamWriter sw2;
            StreamReader sr2;
            String filName = s_strTFAbbrev+"Test.tmp";
            strLoc = "Loc_98v8v";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(null, FileMode.Open);
                iCountErrors++;
                printerr( "Error_987yt! Expected exception not thrown");
                fs2.Close();
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_3981s! Caught Expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_29100! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_48yv";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(String.Empty, FileMode.Open);
                iCountErrors++;
                printerr( "Error_988xh! Expected exception not thrown");
                fs2.Close();
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_398vv! Caught Expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298yx! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_1287z";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(".", FileMode.Open);
                iCountErrors++;
                printerr( "Error_209a8! Expected exception not thrown");
                fs2.Close();
            } 
            catch (UnauthorizedAccessException aexc) 
            {
                printinfo("Info_0989d! Caught Expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1099x! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_1227d";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(filName, ~FileMode.Open);
                iCountErrors++;
                printerr( "Error_299x8! Expected exception not thrown");
                fs2.Close();
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_2099x! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20100! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_98x87";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(filName, FileMode.Open);
                iCountErrors++;
                printerr( "Error_28yv8! Expected exception not thrown");
                fs2.Close();
            } 
            catch (FileNotFoundException aexc) 
            {
                printinfo( "Info_9838v! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_3298y! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_27yvv";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.AutoFlush = true;
            for(char c = 'A' ; c <= 'Z' ; c++)
                sw2.Write(c);
            fs2.Position = 0;
            sr2 = new StreamReader(fs2);
            Int32 tmp;
            Int32 i32 = (Int32)'A';
            while((tmp = sr2.Read()) != -1) 
            {
                iCountTestcases++;
                if(tmp != i32++) 
                {
                    iCountErrors++;
                    printerr ("Error_28v88! Expected=="+(i32-1)+", got value=="+tmp);
                }
            }
            fs2.Close();			
            strLoc = "Loc_2879gs";
            fs2 = new FileStream(filName, FileMode.Open);
            sw2 = new StreamWriter(fs2);
            sw2.AutoFlush = true;
            sr2 = new StreamReader(fs2);
            for(int i = 0 ; i < 10 ; i++)
                sw2.Write(i);
            fs2.Position = 0;
            i32 = '0';
            for(int i = 0 ; i< 10 ;i++) 
            {
                iCountTestcases++;
                if((tmp = sr2.Read()) != i32++) 
                {
                    iCountErrors++;
                    printerr( "Error_27ych_"+i+", Incorrect value in stream, tmp=="+tmp);
                }
            }
            fs2.Close();
            strLoc = "Loc_2908x";
            fs2 = new FileStream(filName, FileMode.Truncate);
            iCountTestcases++;
            if(fs2.Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_20978! Unexpected length on file");
            }			
            sr2 = new StreamReader(fs2);
            iCountTestcases++;
            if(sr2.Read() != -1) 
            {
                iCountErrors++;
                printerr( "Error_89x88! Unexpected value read");
            }
            sw2 = new StreamWriter(fs2);
            for(char c = 'A' ; c < 'K' ; c++) 
                sw2.Write(c);
            fs2.Close();
            strLoc = "Loc_77t8v";
            iCountTestcases++;
            try 
            {
                String strFileName = "ETE_LOGFILE.txt" ; 
                StreamWriter writer = new StreamWriter( strFileName );
                writer.WriteLine("This is a test file");
                writer.Close();  
                FileStream fs = new FileStream(strFileName, FileMode.Append);
                StreamWriter LogStream = new StreamWriter(fs);
                String s = "User logged out at " + DateTime.Now ;
                LogStream.WriteLine(s);
                LogStream.Close();
                fs.Close();
                StreamReader reader = new StreamReader( strFileName );
                String strFileContent = reader.ReadToEnd();
                if( strFileContent.IndexOf( s ) == -1 )
                {
                    printerr( "Error_298yc! File Append is not working correctly");
                    iCountErrors++ ;
                }
                reader.Close();
                if ( File.Exists( strFileName)) 
                    File.Delete( strFileName );
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_209uc! Incorrect exception thrown, exc=="+exc);
            }
            strLoc = "Loc_9y8xq";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(filName, FileMode.CreateNew);
                iCountErrors++;
                printerr( "Error_2988x! Expected exception not thrown");
                fs2.Close();
            } 
            catch (IOException aexc) 
            {
                printinfo( "Info_289x8! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9838x! Incorrect exception thrown, exc=="+exc);
            }
            strLoc = "Loc_7y8xe";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(filName, FileMode.Create | FileMode.Truncate);
                iCountErrors++;
                printerr( "Error_29x88! Expected exception not thrown");
                fs2.Close();
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_388xz Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_7t7yy! Incorrect exception thrown, exc=="+exc.ToString());
            }
            new FileInfo(filName).Delete();
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
        Co5575ctor_str_fm cbA = new Co5575ctor_str_fm();
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
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
