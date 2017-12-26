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
public class Co5588Create_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileInfo.Create";
    public static String s_strTFName        = "co5588create_str.cs";
    public static String s_strTFAbbrev      = "co5588";
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
            FileInfo file2 = null;
            String fileName = s_strTFAbbrev+"Test.tmp";
            try 
            {
                new FileInfo(fileName).Delete();
            } 
            catch (Exception ) {}
            strLoc = "Loc_099u8";
            iCountTestcases++;
            file2 = new FileInfo(".");
            try 
            {
                file2.Create();
                iCountErrors++;
                printerr( "Error_298dy! Expected exception not thrown, file2=="+file2.FullName);
                file2.Delete();
            } 
            catch (UnauthorizedAccessException aexc) 
            {
                printinfo( "Info_298yt! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_209xj! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_498vy";
            iCountTestcases++;                
            fileName =  "Testing";
            file2 = new FileInfo(fileName);
            try 
            {
                FileStream fs = file2.Create();
                if ( file2.Name != fileName  )
                {
                    iCountErrors++;
                    printerr("Error_0010! Unexpected File name :: " + file2.Name );
                }  
                fs.Close();
                file2.Delete();      
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_28829! Unexpected exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_09t83";
            iCountTestcases++;
            file2 = new FileInfo("..\\Test.txt");
            try 
            {
                FileStream fs = file2.Create();
                fs.Close();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0980! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            file2.Delete();
            strLoc = "Loc_2908y";
            StringBuilder sb = new StringBuilder();
            int current = Environment.CurrentDirectory.Length;
            int templength = 260-current-1;
            for(int i = 0 ; i < templength ; i++)
                sb.Append("a");
            iCountTestcases++;
            try 
            {
                file2 = new FileInfo(sb.ToString() );                        
                file2.Create();
                file2.Delete();
                iCountErrors++;
                printerr( "Error_109ty! Expected exception not thrown, file2=="+file2.FullName);
            } 
            catch (PathTooLongException aexc) 
            {
                printinfo( "Error_29010! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_109dv! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_87yg7";
            iCountTestcases++;
            try
            {
                file2 = new FileInfo("!@#$%^&");
                FileStream fs = file2.Create();
                if(!file2.FullName.Equals(Environment.CurrentDirectory+Path.DirectorySeparatorChar+"!@#$%^&")) 
                {
                    iCountErrors++;
                    printerr( "Error_0109x! Incorrect File name, file2=="+file2.FullName);
                }
                fs.Close();
                file2.Delete();
            } 
            catch( Exception e )
            {
                iCountErrors++ ;
                printerr("Unexpected exception occured... " + e.ToString() );
            }
            strLoc = "Loc_209ud";
            iCountTestcases++;
            file2 = new FileInfo(s_strTFPath + "\\" + "Test\\Test\\Test\\test.cs");
            try 
            {
                Console.WriteLine( file2.FullName );
                file2.Create();
                if ( file2.FullName.IndexOf("Test\\Test\\Test")  == -1 )
                {
                    iCountErrors++;
                    printerr("Error_0010! Unexpected File name :: " + file2.FullName );
                }
            } 
            catch( DirectoryNotFoundException e)
            {
                printinfo("Expected exception thrown... " + e.Message  );
            } 
            catch ( Exception exc ) 
            {
                iCountErrors++;
                printerr( "Error_2019u! Incorrect exception thrown, exc=="+exc.ToString());
            }
#if !PLATFORM_UNIX
            strLoc = "Loc_2089x";
            iCountTestcases++;
            try 
            {
                file2 = new FileInfo(":");
                file2.Create();
                iCountErrors++;
                printerr( "Error_19883! Expected exception not thrown, file2=="+file2.FullName);
                file2.Delete();
            } 
            catch ( ArgumentException ) 
            {
            } 
            catch ( Exception exc ) 
            {
                iCountErrors++;
                printerr( "Error_0198xu! Incorrect exception thrown, exc=="+exc.ToString());
            }
#endif //!PLATFORM_UNIX
            strLoc = "Loc_38yc9";
            iCountTestcases++;
            try 
            {
                file2 = new FileInfo("..\\Testfile2.tmp");
                FileStream fs = file2.Create();
                fs.Close();
                file2.Delete();
            } 
            catch ( Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_498yv! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_098gt";
            iCountTestcases++;
            file2 = new FileInfo("..\\..\\test.txt");
            try 
            {
                FileStream fs = file2.Create();
                fs.Close();
                file2.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9092c! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_89ytb";
            try
            {
                String CurrentDirectory2 = Environment.CurrentDirectory;
                file2 = new FileInfo(CurrentDirectory2.Substring(0, CurrentDirectory2.IndexOf("\\")+1)+fileName);
                FileStream fs = file2.Create();
                fs.Close();
                iCountTestcases++;
                if(!file2.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_t78g7! File not created, file=="+file2.FullName);
                }
                file2.Delete();
            } 
            catch( Exception e)
            {
                iCountErrors++ ;
                printinfo("Unexpected exception occured... " + e.ToString() );
            }
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
        Co5588Create_str cbA = new Co5588Create_str();
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
