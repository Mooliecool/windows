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
public class Co9055GetFileNWExtension_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetFileNameWithoutExtension(String)";
    public static String s_strTFName        = "Co9055GetFileNWExtension_str.cs";
    public static String s_strTFAbbrev      = "Co9055";
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
            String strTempFileName;
            String strFileName ;
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                strFileName = Path.GetFileNameWithoutExtension(null);
                if( strFileName != null )
                {
                    iCountErrors++;
                    printerr( "Error_4234! Unexpected return value:" + strFileName);
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
                strFileName = Path.GetFileNameWithoutExtension(String.Empty);
                if ( strFileName != String.Empty )
                {
                    iCountErrors++;
                    printerr( "Error_0001! Unexpected file name" + strFileName);
                }        
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0002! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_89y9t";
            strTempFileName = "jabba\\de\\hutt" ;
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            iCountTestcases++;
            if(strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_9t58y! Incorrect file name returned.. " + strFileName);
            }
            strLoc = "Loc_298yv";
            strTempFileName = "luke..........sky..." ;
            strFileName = Path.GetFileNameWithoutExtension( strTempFileName);
            iCountTestcases++;
            if( strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_298hc! Incorrect file name=="+strFileName);
            }
            strLoc = "loc_48yvy";
            strTempFileName = "star/wars/han.solo" ;
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            iCountTestcases++;
            if(strFileName != GetFileName( strTempFileName)) 
            {
                iCountErrors++;
                printerr( "Error_298ch! Incorrect file name=="+strFileName);
            }
            strLoc = "Err_1003";
            iCountTestcases++;
            strTempFileName =  "foo.bar.fkl;fkds92-509450-4359.$#%()#%().%#(%)_#(%_)";
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            if( strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_1004! Incorrect Incorrect file name =="+strFileName);
            }
            strLoc = "Err_2003";
            iCountTestcases++;
            String extension = ".bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            strTempFileName =  "AAAAAAAAAAAAAAAAAAAAAA" + extension ;
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            if( strFileName != GetFileName( strTempFileName)) 
            {
                iCountErrors++;
                printerr( "Error_2004! Incorrect file name=="+strFileName);
            }
            strLoc = "Err_3003";
            iCountTestcases++;
            strTempFileName =  "foo..............cs";                     
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);                          
            if( strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_3004! Incorrect file name=="+strFileName);
            }
            strLoc = "Err_4003";
            iCountTestcases++;
            strTempFileName =  ".$#@$_)+_)!@@!!@##&_$)#_";
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            if( strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_4004! Incorrect file name =="+strFileName);
            }
#if !PLATFORM_UNIX
            strLoc = "Err_5003";
            iCountTestcases++;
            strTempFileName =  @"\\foo\bar\test\whatever\\\\" ;
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            if( strFileName != String.Empty ) 
            {
                iCountErrors++;
                printerr( "Error_5004! Incorrect file name=="+strFileName);
            }
#endif
            strLoc = "Err_6003";
            iCountTestcases++;
            strTempFileName =  "foo.z" ; 
            strFileName = Path.GetFileNameWithoutExtension(strTempFileName);
            if( strFileName != GetFileName( strTempFileName) ) 
            {
                iCountErrors++;
                printerr( "Error_6004! Incorrect file name=="+strFileName);
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
        Co9055GetFileNWExtension_str cbA = new Co9055GetFileNWExtension_str();
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
