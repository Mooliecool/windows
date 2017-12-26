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
using System.Security;
using System.Security.Permissions;
public class Co9056GetFullPath_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetFullPath(String)";
    public static String s_strTFName        = "Co9056GetFullPath_str.cs";
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
            String str1, strRet;
            strLoc = "Loc_00001";
            iCountTestcases++;
            try 
            {
                strRet = Path.GetFullPath(null);
                iCountErrors++;
                printerr( "Error_00002! Expected exception not thrown, strRet=="+strRet);
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Info_00003! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00004! Incorrect exception thrown, exc=="+exc.ToString());
            }			
            strLoc = "Loc_00005";
            iCountTestcases++;
            try 
            {
                strRet = Path.GetFullPath("");
                iCountErrors++;
                printerr( "Error_00006! Expected exception not thrown, strRet=="+strRet);
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_00007! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00008! Incorrect exception thrown, exc=="+exc.ToString());
            }
#if PLATFORM_UNIX
			strLoc = "Loc_00009";
			str1 = "/Directory/SubDirectory";
			strRet = Path.GetFullPath(str1);
			iCountTestcases++;
			if(!strRet.Equals(str1)) {
				iCountErrors++;
				printerr( "Error_00010! Expected==<"+str1+"> strRet==<"+strRet+">");
			} 
#else
            strLoc = "Loc_00009";
            str1 = "C:\\Directory\\SubDirectory";
            strRet = Path.GetFullPath(str1);
            iCountTestcases++;
            if(!strRet.Equals(str1)) 
            {
                iCountErrors++;
                printerr( "Error_00010! Expected==<"+str1+"> strRet==<"+strRet+">");
            } 
#endif
            strLoc = "Loc_00011";
            str1 = "Directory" + Path.DirectorySeparatorChar + "SubDirectory";
            strRet = Path.GetFullPath(str1);
            iCountTestcases++;
            if(!strRet.Equals(Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar +str1)) 
            {
                iCountErrors++;
                printerr( "Errror_00012! Incorrect string=="+strRet);
            }
#if !PLATFORM_UNIX
            strLoc = "Loc_00013";
            str1 = "\\\\fxqasql\\TestDrivers\\FxBcl\\IO";
            strRet = Path.GetFullPath(str1);
            iCountTestcases++;
            if(!strRet.Equals(str1)) 
            {
                iCountErrors++;
                printerr("Error_00014! Incorrect fullstring=="+strRet);
            }
#endif
            strLoc = "Loc_00014a";
            try
            {
                Console.WriteLine(Path.AltDirectorySeparatorChar);
#if PLATFORM_UNIX
        		new FileIOPermission(FileIOPermissionAccess.PathDiscovery, @"/foo/bar").Deny();
        		String path = Path.GetFullPath(@"/foo/bar"); 
#else
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, @"c:\foo\bar").Deny();
                String path = Path.GetFullPath(@"c:\\foo\\bar"); 
#endif
                Console.WriteLine(path);
            } 
            catch ( SecurityException )
            {
            } 
            catch ( Exception e )
            {
                iCountErrors++ ;
                Console.WriteLine( "Error_00014b!!! Incorrest exception occured... msg :" + e.Message );
            }
            strLoc = "Loc_00015";
            strRet = Path.GetFullPath(".");
            iCountTestcases++;
            if(!strRet.Equals(Directory.GetCurrentDirectory())) 
            {
                iCountErrors++;
                printerr( "Error_00016! Incorrect fullstring=="+str1);
            }
            strLoc = "Loc_00017";
            strRet = Path.GetFullPath("..");
            iCountTestcases++;
            str1 = Directory.GetCurrentDirectory().Substring(0, Directory.GetCurrentDirectory().LastIndexOf(Path.DirectorySeparatorChar));
            if(str1.Length == 2) str1 += Path.DirectorySeparatorChar;
            if(!strRet.Equals(str1)) 
            {
                iCountErrors++;
                printerr( "Error_00018! Incorrect fullstring=="+str1);
            }
            strLoc = "Loc_00019";
            str1 = String.Empty;
            for(int i = 0 ; i < 270 ; i++)
                str1 += "a";
            iCountTestcases++;
            try 
            {
                strRet = Path.GetFullPath(str1);
                iCountErrors++;
                printerr( "Error_3989f! Expected exception not thrown, strRet=="+strRet);
            } 
            catch (PathTooLongException pexc) 
            {
                printinfo( "Info_38gy8! Caught expected exception, pexc=="+pexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_38y8v! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_00021";
            iCountTestcases++;
            try 
            {
                strRet = Path.GetFullPath("Hello*");
                iCountErrors++;
                printerr("Error_00022! Expected exception not thrown, strRet=="+strRet);
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_00023! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00024! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_00025";
            iCountTestcases++;
            strRet = Path.GetFullPath("." + Path.DirectorySeparatorChar);
            iCountTestcases++;
            if(!strRet.Equals(Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar)) 
            {
                iCountErrors++;
                printerr( "Error_00026! Incorrect fullstring=="+strRet);
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString() );
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
        Co9056GetFullPath_str cbA = new Co9056GetFullPath_str();
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
