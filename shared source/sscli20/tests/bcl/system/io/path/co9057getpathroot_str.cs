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
public class Co9057GetPathRoot_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetPathRoot(String)";
    public static String s_strTFName        = "co9057getpathroot_str.cs";
    public static String s_strTFAbbrev      = "co9057";
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
            String strPathName ;
            String strPathRoot ;
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                strPathName = Path.GetPathRoot(null);
                if( strPathName != null )
                {
                    iCountErrors++;
                    printerr( "Error_4234! Unexpected return value:" + strPathName);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_019ux! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_0000";
            iCountTestcases++;
            try 
            {
                strPathRoot = Path.GetPathRoot(String.Empty);
                iCountErrors++;
                printerr( "Error_0001! Unexpected path root" + strPathRoot);
            } 
            catch (ArgumentException )
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0002! Incorrect exception thrown, exc=="+exc.ToString());
            }
#if !PLATFORM_UNIX
            strLoc = "Loc_89y9t";
            strPathName = @"\\jabba\de\hutt" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != @"\\jabba\de" ) 
            {
                iCountErrors++;
                printerr( "Error_9t58y! incorrect path root ::" + strPathRoot);
            }
#endif
            strLoc = "Loc_0493";
#if !PLATFORM_UNIX
            strPathName = @"C:\" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != @"C:\" ) 
            {
                iCountErrors++;
                printerr( "Error_4390! incorrect path root ::" + strPathRoot);
            }
            strLoc = "Loc_9979";
            strPathName = @"C:\\\\\\\foo\\\\\\\\\\\\\\\\bar\\\\\\\\\\\\test" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != @"C:\" ) 
            {                  
                iCountErrors++;
                printerr( "Error_9979! incorrect path root ::" + strPathRoot);
            }
#endif
            strLoc = "Loc_298yv";
            strPathName = @"C\" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != String.Empty ) 
            {                  
                iCountErrors++;
                printerr( "Error_58985! incorrect path root ::" + strPathRoot);
            }
            strLoc = "Loc_298yv";
            strPathName = "C" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != String.Empty ) 
            {                  
                iCountErrors++;
                printerr( "Error_9898! incorrect path root ::" + strPathRoot);
            }
            strLoc = "Loc_298yv";

            try {
                strPathName = @":\" ;
                strPathRoot = Path.GetPathRoot(strPathName);
                iCountTestcases++;
                if(strPathRoot != String.Empty ) 
                {                  
                    iCountErrors++;
                    printerr( "Error_4344! incorrect path root ::" + strPathRoot);
                }
            } catch (ArgumentException) {
            } catch (Exception e) {
                Console.WriteLine("Error_0590!!!! Unexpected exception occured... Exception message:" + e.ToString() );
                iCountErrors++ ;
            }
            strLoc = "loc_48yvy";
            strPathName = @"\test\test\test" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
#if PLATFORM_UNIX
			if(strPathRoot != "/" ) 
#else
            if(strPathRoot != "\\" ) 
#endif
            {
                iCountErrors++;
                printerr( "Error_298ch! Extension=="+strPathRoot);
            }
            strLoc = "loc_5438";
#if !PLATFORM_UNIX
            strPathName = @"c:test\test\test" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != "c:" ) 
            {
                iCountErrors++;
                printerr( "Error_5043! Extension=="+strPathRoot);
            }
#endif
            strLoc = "loc_43894";
            strPathName = @"C.\" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
            if(strPathRoot != String.Empty ) 
            {
                iCountErrors++;
                printerr( "Error_43244! Extension=="+strPathRoot);
            }
            strLoc = "loc_58659";
            strPathName = @"\" ;
            strPathRoot = Path.GetPathRoot(strPathName);
            iCountTestcases++;
#if PLATFORM_UNIX
            if(strPathRoot != @"/") 
#else
            if(strPathRoot != @"\") 
#endif
            {
                iCountErrors++;
                printerr( "Error_08348! Extension=="+strPathRoot);
            }
            strLoc = "Err_1003";
            iCountTestcases++;
            strPathName =  "foo.bar.fkl;fkds92-509450-4359.$#%()#%().%#(%)_#(%_)";
            strPathRoot = Path.GetPathRoot(strPathName);
            if( strPathRoot != String.Empty ) 
            {
                iCountErrors++;
                printerr( "Error_1004! Incorrect extension , file=="+strPathRoot);
            }
#if !PLATFORM_UNIX
            strLoc = "Err_5003";
            try
            {
                iCountTestcases++;
                strPathName =  @"\\\\\\\\\\" ;
                strPathRoot = Path.GetPathRoot(strPathName);
                if( strPathRoot != @"\\" ) 
                {
                    iCountErrors++;
                    printerr( "Error_5004! Incorrect extension , file=="+strPathRoot);
                }
            } 
            catch( ArgumentException )
            {
            } 
            catch( Exception )
            {
                iCountErrors++;
                printerr( "Error_5894! Incorrect exception occured");
            }
#endif
            strLoc = "Err_6003";
            iCountTestcases++;
            strPathName =  @"               c:\";
            strPathRoot = Path.GetPathRoot(strPathName);
#if !PLATFORM_UNIX
            if( strPathRoot != @"c:\") 
#else
            // On Unix @"               c:\" is a valid relative path name,
            // therefore the path root is an empty string
            if( strPathRoot != "") 
#endif
            {
                iCountErrors++;
                printerr( "Error_6004! Incorrect extension , file=="+strPathRoot);
            }
#if !PLATFORM_UNIX
            strLoc = "Err_3279";
            iCountTestcases++;
            strPathName =  @"\\Radqa\vbtestlib";
            strPathRoot = Path.GetPathRoot(strPathName);
            if( strPathRoot != @"\\Radqa\vbtestlib"  ) 
            {
                iCountErrors++;
                printerr( "Error_4343! Incorrect extension , file=="+strPathRoot);
            }
            strLoc = "Err_5439";
            try
            {
                iCountTestcases++;
                strPathName =  @"\\Radqa";
                strPathRoot = Path.GetPathRoot(strPathName);
                if( strPathRoot != @"\\Radqa" ) 
                {
                    iCountErrors++;
                    printerr( "Error_42412! Incorrect extension , file=="+strPathRoot);
                }
            } 
            catch( ArgumentException )
            {
            } 
            catch( Exception )
            {
                iCountErrors++;
                printerr( "Error_fdsf! Incorrect exception occured");
            }
#endif
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
    private String GetFileName(String str1)
    {
        if ( str1.LastIndexOf(@"\") != -1 ) 
            str1 = str1.Substring( str1.LastIndexOf(@"\") + 1) ;
        if ( str1.LastIndexOf( @"/") != -1 )  
            str1 = str1.Substring( str1.LastIndexOf(@"/") + 1) ;
        if (  str1.LastIndexOf(".") != -1 )   
            str1 = str1.Substring( 0 ,  str1.LastIndexOf("."));      
        return str1;         
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
        Co9057GetPathRoot_str cbA = new Co9057GetPathRoot_str();
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
